/*
 * Convert CV++ Assembler Output to work with MASM
 * Written by philip0000000
 * Find the project here [https://github.com/philip0000000/visual-cpp-Assembler-Output-to-MASM]
 */
#include <vector>
#include <string>
#include <iostream>

#include "constants.h"
#include "CLParser.h"
#include "CleanFileForMASM.h"
#include "CompileWithMASM.h"


 /*
  *  TODO: add -cl (dose both c and l)
  */


  // print help message
void Help()
{
    std::cout << PROJECT_NAME << " v." << VERSION << NEWLINE << NEWLINE
        << "Converts x86-64 assembly from Visual C++ Assembler Output to work with Microsoft Macro Assembler(MASM) 64-bit(ML64) "
        "and compile with linking to static library files." << NEWLINE;
    std::cout << NEWLINE << "Arguments:" << NEWLINE
        << TAB << "-h" << NEWLINE
        << TAB << TAB << "for help info(this info)" << NEWLINE
        << TAB << "-c <input .asm file name> <output .asm file name>" << NEWLINE
        << TAB << TAB << "comment out and replace the necessary things for the file to compile with MASM" << NEWLINE
        << TAB << "-l <input .asm file> <PATH to .log file> <PATH to ml64.exe>" << NEWLINE
        << TAB << TAB << "try to compile the file until it has succeeded" << std::endl;
}

int main(int argc, char** argv)
{
    std::vector<FlagValue> FlagValues = { {"-c", 2},
                                          {"-l", 3},
                                          {"-h", 0} };

    CLParser input(argc, argv, FlagValues);

    if (input.NumberOfTokens() > 0)
    {
        if (input.FlagExist("-c") == true && input.FlagExist("-l") == false)
        {
            std::vector<std::string> arguments = input.getFlagValue("-c");
            if (arguments.empty() == false)
            {
                CleanFileForMASM CleanFile;
                CleanFile.ProcessFile(arguments[0], arguments[1]);
            }
            else
            {
                // error
                std::cerr << "ERROR!" << NEWLINE << "-c flag had wrong arguments!" << std::endl;
            }
        }
        else if (input.FlagExist("-c") == false && input.FlagExist("-l") == true)
        {
            std::vector<std::string> arguments = input.getFlagValue("-l");
            if (arguments.empty() == false)
            {
                CompileWithMasm CompileMASM;
                if (CompileMASM.Compile(arguments[0], arguments[1], arguments[2]))
                {
                    // ERROR
                    return 1;
                }
            }
            else
            {
                // error
                std::cerr << "ERROR!" << NEWLINE << "-l flag had wrong arguments!" << NEWLINE << "- h for help" << std::endl;
            }
        }
        else if (input.FlagExist("-h"))
        {
            Help();
        }
        else
        {
            // error
            std::cerr << "ERROR!" << NEWLINE << "No correct flag was entered!" << NEWLINE << "- h for help" << std::endl;
        }
    }
    else
    {
        Help();
    }

    return 0;
}
