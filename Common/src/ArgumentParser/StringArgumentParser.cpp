#include "StringArgumentParser.hpp"

#include <regex>

namespace nCommon
{

    void StringArgumentParser::ParseValue(const ::std::string &iValue)
    {
        std::regex reg("^[a-zA-Z_]+");
        if (std::regex_match(iValue, reg))
            mValue = iValue;
        else
            mValue = mDefaultValue;
    }


} // nCommon