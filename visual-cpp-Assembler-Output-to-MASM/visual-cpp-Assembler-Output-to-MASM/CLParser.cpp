#include <vector>
//#include <string>
//#include <algorithm>

#include "CLParser.h"


// https://thispointer.com/using-stdfind-stdfind_if-with-user-defined-classes/
struct FlagValueComparator
{
    std::string m_FlagName;
    FlagValueComparator(std::string str) :
        m_FlagName(str)
    {}
    bool operator()(const FlagValue& fv)
    {
        if (fv.FlagName == m_FlagName)
            return true;
        return false;
    }
};


CLParser::CLParser(int& argc, char** argv, std::vector<FlagValue>& fv) :
    FlagValues(fv)
{
    // save all tokens
    for (int n = 1; n < argc; n++)
        this->Tokens.push_back(std::string(argv[n]));
}

int CLParser::NumberOfTokens() const
{
    return Tokens.size();
}

bool CLParser::FlagExist(const std::string& flag) const
{
    // 1st - check if value exist
    auto result0 = std::find(this->Tokens.begin(), this->Tokens.end(), flag);
    if (result0 != this->Tokens.end())
    {
        // 2nd - check if flag is acceptable value
        auto result1 = std::find_if(FlagValues.begin(), FlagValues.end(), FlagValueComparator(std::string(flag)));
        if (result1 != FlagValues.end())
        {
            return true;
        }
    }

    // Item not Found
    return false;
}
std::vector<std::string> CLParser::getFlagValue(const std::string& flag, bool CheckForOtherFlags) const
{
    std::vector<std::string> returnValue;

    // 1st - check if value exist
    auto result0 = std::find(this->Tokens.begin(), this->Tokens.end(), flag);
    if (result0 != this->Tokens.end())
    {
        // 2nd - get the amount of arguments the flag has
        auto result1 = std::find_if(FlagValues.begin(), FlagValues.end(), FlagValueComparator(std::string(flag)));
        if (result1 != FlagValues.end())
        {
            int NumberOfArgumentsFlagHas = result1->NumberOfArguments;

            // add number of arguments to return value
            for (result0++; NumberOfArgumentsFlagHas > 0 && result0 != this->Tokens.end(); result0++, NumberOfArgumentsFlagHas--)
            {
                // if we find a argument with '-' at the begining, we consider it to be a flag and break the loop
                if (CheckForOtherFlags == true && (*result0)[0] == '-')
                    break;
                returnValue.push_back(*result0);
            }

            // if we did not get all arguments, we return empty, as error indicator that not enough arguments were provided
            if (NumberOfArgumentsFlagHas > 0)
                returnValue.clear();
        }
    }

    return returnValue;
}
