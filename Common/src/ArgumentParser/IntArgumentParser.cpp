#include "IntArgumentParser.hpp"

#include <regex>

namespace nCommon
{

    void IntArgumentParser::ParseValue(const std::string &iValue)
    {
        std::regex reg("^[0-9]+");

        if (std::regex_match(iValue, reg))
            mValue = std::stoi(iValue);
        else
            mValue = std::stoi(mDefaultValue);
    }


} //nCommon