#include <fstream>
#include <string>
#include <iostream>

#include "CleanFileForMASM.h"
#include "constants.h"


enum class StringCode
{
    NOT_FOUND,
    LISTING_INC,
    OLDNAMES,
    FLAT_SINGLE_SEMICOLON,
    PDATA_SEGMENT,
    REX_JMP,
    NPAD10
};
StringCode HashString(std::string const& inString) {
    if (inString == "include listing.inc") return StringCode::LISTING_INC;
    if (inString == "INCLUDELIB OLDNAMES") return StringCode::OLDNAMES;
    if (inString.find(std::string("FLAT:")) != std::string::npos) return StringCode::FLAT_SINGLE_SEMICOLON;
    if (inString.find(std::string("pdata\tSEGMENT")) != std::string::npos) return StringCode::PDATA_SEGMENT;
    if (inString.find(std::string("rex_jmp")) != std::string::npos) return StringCode::REX_JMP;
    if (inString.find(std::string("npad\t10")) != std::string::npos) return StringCode::NPAD10;
    return StringCode::NOT_FOUND;
}


bool CleanFileForMASM::ProcessFile(std::string FileToProcess, std::string SaveFileAs)
{
    OpenFile(FileToProcess);
    ParseData();
    SaveFile(SaveFileAs);

    return true;
}

void CleanFileForMASM::OpenFile(std::string file)
{
    std::ifstream infile;
    std::string line;

    infile.open(file);
    if (!infile.is_open())
    {
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    while (std::getline(infile, line))
    {
        InputFileData.push_back(line);
    }

    infile.close();
}
void CleanFileForMASM::ParseData()
{
    auto str = InputFileData.begin(), end = InputFileData.end();
    while (str != end)
    {
        switch (HashString(*str))
        {
        case StringCode::LISTING_INC:
        case StringCode::OLDNAMES:
            CommentString(*str);
            ++str;
            break;
        case StringCode::FLAT_SINGLE_SEMICOLON:
            RemoveFromString(*str, std::string("FLAT:"));
            ++str;
            break;
        case StringCode::PDATA_SEGMENT:
            CommentUntil(str, std::string("pdata\tENDS"), end);
            break;
        case StringCode::REX_JMP:
            ReplaceStringWith(*str, std::string("rex_jmp"), std::string("jmp"));
            ++str;
            break;
        case StringCode::NPAD10:
            AddNope(15);
            ++str;
            break;
        case StringCode::NOT_FOUND:
        default:
            AddLineToOutput(*str);
            ++str;
            break;
        }
    }
}
void CleanFileForMASM::SaveFile(std::string file)
{
    std::ofstream outfile;
    outfile.open(file);

    // save data that has been manipulated to file
    if (outfile.is_open())
    {
        if (OutputFileData.empty() == false)
        {
            for (std::string str : OutputFileData)
            {
                outfile << str << LF;
            }
        }

        outfile.close();
    }
    else
        std::cerr << "Unable to open file: " << file << std::endl;
}

void CleanFileForMASM::AddLineToOutput(std::string& str)
{
    OutputFileData.push_back(str);
}
void CleanFileForMASM::CommentString(std::string& str)
{
    OutputFileData.push_back(';' + str);
}
void CleanFileForMASM::RemoveFromString(std::string& str, const std::string& out)
{
    // Search for the substring in string
    size_t pos = str.find(out);

    if (pos != std::string::npos)
    {
        // found out in str, remove out
        str.erase(pos, out.length());

        // Add what was removed as comment
        str += " ;" + out;

        OutputFileData.push_back(str);
    }
}
void CleanFileForMASM::CommentUntil(std::vector<std::string>::iterator& it, const std::string& until, std::vector<std::string>::iterator& end)
{
    // comment out the 1st string
    CommentString(*it);
    ++it;

    // comment out until we find parameter string until
    while (it != end && it->find(until) == std::string::npos)
    {
        CommentString(*it);
        ++it;
    }

    // comment out also the string with until, if we are not at the end
    if (it != end)
    {
        CommentString(*it);
        ++it;
    }
}
void CleanFileForMASM::ReplaceStringWith(std::string& str, const std::string& replace, const std::string& replaceWith)
{
    size_t pos = str.find(replace);
    if (pos != std::string::npos)
    {
        str.replace(pos, replace.length(), replaceWith);
        OutputFileData.push_back(str);
    }
}

void CleanFileForMASM::AddNope(int n)
{
    for (; n > 0; n--)
    {
        OutputFileData.push_back(std::string("\tnop"));
    }
}
