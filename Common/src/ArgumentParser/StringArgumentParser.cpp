#include "StringArgumentParser.hpp"

#include <iostream>
#include <regex>

namespace nCommon
{

    void StringArgumentParser::ParseValue(const ::std::string& iValue)
    {
        std::regex reg("^[a-zA-Z_]+");
        if (std::regex_match(iValue, reg))
            mValue = iValue;
        else
        {
            std::cout<<"Bad value for argument: "<<mID<<std::endl;
            mValue = mDefaultValue;
        }
    }


} // nCommon