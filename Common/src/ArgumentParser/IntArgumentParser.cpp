#include "IntArgumentParser.hpp"

#include <iostream>
#include <regex>

namespace nCommon
{

    void IntArgumentParser::ParseValue(const std::string& iValue)
    {
        std::regex reg("^-*[0-9]+");

        if (std::regex_match(iValue, reg))
            mValue = std::stoi(iValue);
        else
        {
            std::cout<<"Bad value for argument: "<<mID<<std::endl;
            mValue = std::stoi(mDefaultValue);
        }
    }


} //nCommon