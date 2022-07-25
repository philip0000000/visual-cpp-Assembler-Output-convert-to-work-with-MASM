#pragma once
#ifndef PARSE_FLAGS_H
#define PARSE_FLAGS_H


#include <string>


struct FlagValue
{
    std::string FlagName;
    int NumberOfArguments;
};


// Command-line argument parser
class CLParser
{
public:
    // Parse the command line, and the flag values with respected arguments.
    CLParser(int& argc, char** argv, std::vector<FlagValue>& fv);

    // Returns total number of arguments.
    int NumberOfTokens() const;

    // Returns true if flag exist, else false.
    bool FlagExist(const std::string& option) const;
    // Returns number of arguments. If flag could not be found or
    // not enough arguments, return empty vector.
    // If CheckForOtherFlags ia true, will return empty if encounter "-" at the beging of a argument.
    // After the flag(check the size of the flag arguemnt size).
    std::vector<std::string> getFlagValue(const std::string& flag, bool CheckForOtherFlags = true) const;

private:
    std::vector<FlagValue> FlagValues;
    std::vector<std::string> Tokens;
};


#endif /* PARSE_FLAGS_H */
