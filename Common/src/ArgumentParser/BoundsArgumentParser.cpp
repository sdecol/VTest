#include "BoundsArgumentParser.hpp"

#include <cassert>
#include <iostream>
#include <regex>

namespace nCommon
{
    BoundsArgumentParser::BoundsArgumentParser(const std::string& iID, const std::string& iDefaultValue) :
            ArgumentParserTemplate<std::pair<int, int>>(iID, iDefaultValue)
    {
        mValue = ParseBounds(mDefaultValue);
    }

    void BoundsArgumentParser::ParseValue(const std::string& iValue)
    {
        std::regex reg("^-*[0-9]+,-*[0-9]+");

        if (std::regex_match(iValue, reg))
            mValue = ParseBounds(iValue);
        else
        {
            std::cout<<"Bad value for argument: "<<mID<<std::endl;
        }
    }


    std::pair<int, int> BoundsArgumentParser::ParseBounds(const std::string& iValue)
    {
        std::stringstream stream(iValue);
        std::string strValue;
        std::vector<std::string> bounds;

        while (std::getline(stream, strValue, ','))
        {
            bounds.push_back(strValue);
        }

        assert(bounds.size() == 2);

        return {std::stoi(bounds[0]), std::stoi(bounds[1])};
    }

}