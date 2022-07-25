#include <fstream>
#include <Windows.h>
#include <atlstr.h>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "CompileWithMasm.h"


bool CompileWithMasm::Compile(std::string& ASMFile, std::string& PATHLogFile, std::string& PATHToML64)
{
    // 1st - parse .log file of the VC++ project
    ParseLogFile(PATHLogFile);

    // 2nd - try and compile the file
    // add the minimal data to be able to compile with ml64.exe
    AddQuotationMarkToString(PATHToML64);
    AddQuotationMarkToString(ASMFile);
    CommandLineInput = PATHToML64 + SPACE + ASMFile + SPACE + DEFAULT_COMPILE_VALUE;

    std::string LastStaticLibraryLinked = "";
    bool bRunLoop = true;
    bool bError = false;

    while (bRunLoop)
    {
        std::string output = InvokeCommandAndGetOutput(CommandLineInput);

        // 1st - check if we succeded(found no errors)
        if (output.rfind(ERROR_MSG) == std::string::npos)
        {
            // we succeeded without any errors
            bRunLoop = false;
            std::cout << TAB << "succeeded to compile the program with this command:" << NEWLINE
                << CommandLineInput << std::endl;
        }
        else
        {
            // 2nd - check if we have error LNK2019, if yes, try to correct theme and try again
            if (output.rfind(UNRESOLVED_EXTERNAL_SYMBOL) != std::string::npos)
            {
                // extract the function that needs to be linked
                std::string ExternalFunction;

                // find begining of external function
                size_t foundBegin = output.find(UNRESOLVED_EXTERNAL_SYMBOL);
                foundBegin += sizeof(UNRESOLVED_EXTERNAL_SYMBOL) - 1;

                // find end of external function
                size_t foundEnd = foundBegin;
                // go to next space or CR or end of string
                while (foundEnd < output.size() && output[foundEnd] != SPACE && output[foundEnd] != CR)
                {
                    foundEnd++;
                }
                if (foundEnd >= output.size())
                {
                    bool bRunLoop = false;
                    bool bError = true;
                    std::cerr << "ERROR!" << NEWLINE
                        << "Not enough space";
                    break;
                }

                if (foundEnd > foundBegin)
                {
                    size_t length = foundEnd - foundBegin;

                    // extract the external function
                    if (length > 0)
                    {
                        ExternalFunction = output.substr(foundBegin, length);

                        // get static library that has specific external function
                        std::string StaticLibrary = FindStaticLibraryWithFunctionReferended(ExternalFunction);
                        if (StaticLibrary.empty() == false)
                        {
                            AddQuotationMarkToString(StaticLibrary);

                            if (LastStaticLibraryLinked != StaticLibrary)
                            {
                                // found a new static library to link with, try again to compile
                                CommandLineInput += SPACE;
                                CommandLineInput += DEFAULTLIB + StaticLibrary;
                                LastStaticLibraryLinked = StaticLibrary;

                                //std::cerr << CommandLineInput;
                            }
                            else
                            {
                                bRunLoop = false;
                                bError = true;
                                std::cerr << "ERROR!" << NEWLINE
                                    << "have allready tried to compile with static library without success, static library: "
                                    + StaticLibrary << std::endl;
                            }
                        }
                        else
                        {
                            bRunLoop = false;
                            bError = true;
                            std::cerr << "ERROR!" << NEWLINE
                                << "could not find a static library to the external function: " + ExternalFunction << std::endl;
                        }
                    }
                    else
                    {
                        bRunLoop = false;
                        bError = true;
                        std::cerr << "ERROR!" << NEWLINE
                            << "external function: " + ExternalFunction + ". Did not have any length." << std::endl;
                    }
                }
                else
                {
                    bRunLoop = false;
                    bError = true;
                    std::cerr << "ERROR!" << NEWLINE
                        << "could not find external function" << std::endl;
                }
            }
            else
            {
                bRunLoop = false;
                bError = true;
                std::cerr << "ERROR!" << NEWLINE
                    << TAB << "could not find the problem, output:" << NEWLINE
                    << output;
            }
        }
    }

    return bError;
}

void CompileWithMasm::ParseLogFile(std::string& PATHLogFile)
{
    std::ifstream infile;
    std::string line;
    std::vector<std::string> LogFile;

    infile.open(PATHLogFile);
    if (!infile.is_open())
    {
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    while (std::getline(infile, line))
    {
        LogFile.push_back(line);
    }

    infile.close();

    if (LogFile.empty() == false)
    {
        // 1st - find "      Searching "
        // 2nd - add searching to map, only if after is "        Found "
        // 3rd - add all "        Found " until "      Searching ", repeat then

        auto data = LogFile.begin(), end = LogFile.end();
        std::string SearchingStr;

        while (data != end)
        {
            if (data->rfind(SEARCHING_LOG_FILE, 0) != std::string::npos)
            {
                // if next line has FOUND_LOG_FILE
                data++;
                if (data->rfind(FOUND_LOG_FILE, 0) != std::string::npos)
                {
                    // add SEARCHING_LOG_FILE
                    data--;
                    std::string StaticLibraryPath(*data);
                    // remove beginning of data("      Searching ")
                    StaticLibraryPath.erase(0, SEARCHING_LOG_FILE_SIZE);
                    // remove the last character(":")
                    StaticLibraryPath.pop_back();

                    // add "        Found "
                    data++;
                    std::string ReferencedFunction(*data);
                    // remove beginning of data("        Found ")
                    ReferencedFunction.erase(0, FOUND_LOG_FILE_SIZE);

                    // add static library to map
                    FunctionReferendedInStaticLibraryTree[StaticLibraryPath].push_back(ReferencedFunction);

                    // add all the rest of referenced function, until EOF or new "      Searching " found
                    for (data++;
                        data != end && data->rfind(SEARCHING_LOG_FILE, 0) == std::string::npos;
                        data++)
                    {
                        if (data->rfind(FOUND_LOG_FILE, 0) != std::string::npos)
                        {
                            std::string ReferencedFunction(*data);
                            // remove beginning of data("        Found ")
                            ReferencedFunction.erase(0, FOUND_LOG_FILE_SIZE);

                            // add data to map
                            FunctionReferendedInStaticLibraryTree[StaticLibraryPath].push_back(ReferencedFunction);
                        }
                    }
                }
            }
            else
                data++;
        }
    }
}

std::string CompileWithMasm::InvokeCommandAndGetOutput(std::string Command)
{
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFOA si;
    BOOL bReturn;

    std::string ReturnValue;
    HANDLE hPipeInput, hPipeOutput;

    // initialize sa
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE; // new process inherits from child

    // create pipe to get output from process
    bReturn = CreatePipe(&hPipeInput, &hPipeOutput, &sa, 0);
    if (bReturn == FALSE)
    {
        return ReturnValue;
    }

    // initialize sa and pi
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE; // hides the process, need STARTF_USESHOWWINDOW
    si.hStdOutput = hPipeOutput;
    si.hStdError = hPipeOutput;

    bReturn = CreateProcessA(NULL, (LPSTR)Command.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (bReturn == FALSE)
    {
        // process was not created
        CloseHandle(hPipeOutput);
        CloseHandle(hPipeInput);
        return ReturnValue;
    }

    // wait for process to finish, check every 50 milliseconds, if not finished wait again
    DWORD ReturnEvent;
    do {
        ReturnEvent = WaitForSingleObject(pi.hProcess, 50);
    } while (ReturnEvent == WAIT_TIMEOUT);

    // true if process finished
    if (ReturnEvent == WAIT_OBJECT_0)
    {
        // store the output in the returning string
        CHAR buffer[5024];
        DWORD CharactersRead = 0;

        // read the output from the process
        PeekNamedPipe(hPipeInput, buffer, sizeof(buffer), &CharactersRead, NULL, NULL);

        // add zero-terminated character
        buffer[CharactersRead - 1] = ZERO_TERMINATED_CHARACTER;

        // add output to return value
        ReturnValue += buffer;
    }
    //else
        // something went wrong with the process

    // cleanup
    CloseHandle(hPipeOutput);
    CloseHandle(hPipeInput);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return ReturnValue;
}
void CompileWithMasm::AddQuotationMarkToString(std::string& str)
{
    // check if start of string has quotation mark, if not, add quotation mark
    if (str[0] != QUOTATION_MARK)
    {
        str.insert(0, 1, QUOTATION_MARK);
    }

    // check if end of string has quotation mark, if not, add quotation mark
    if (str.back() != QUOTATION_MARK)
    {
        str += QUOTATION_MARK;
    }
}
std::string CompileWithMasm::FindStaticLibraryWithFunctionReferended(std::string& FunctionReferended)
{
    std::string str;

    // loop through all keys, and check if value vector has FunctionReferended, if yes, return that key
    for (auto const& strVector : FunctionReferendedInStaticLibraryTree)
    {
        if (std::find(strVector.second.begin(), strVector.second.end(), FunctionReferended) != strVector.second.end())
        {
            str = strVector.first;
            break;
        }
    }

    return str;
}
