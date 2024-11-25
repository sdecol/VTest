#include "ArgumentParser.hpp"

#include <iostream>
#include <stdexcept>
#include <regex>


namespace nCommon
{
    ArgumentParser::ArgumentParser(const std::string &iID, ArgumentParser::ArgumentType iType,
                                   const std::string &iDefaultValue) : mID(iID),
                                                                       mType(iType),
                                                                       mDefaultValue(iDefaultValue)
    {
        assert(!iDefaultValue.empty());
        mValue = iDefaultValue;
    }


    void ArgumentParser::ParseValue(const std::string &iValue)
    {
        mValue = mDefaultValue;

        bool isValid = true;

        if (mType == ArgumentParser::ArgumentType::String) {
            std::regex reg("^[a-zA-Z_]+");
            if (!std::regex_match(iValue, reg))
                isValid = false;
        } else if (mType == ArgumentParser::ArgumentType::Number) {
            std::regex reg("^[0-9]+");
            if (!std::regex_match(iValue, reg))
                isValid = false;
        } else if (mType == ArgumentParser::ArgumentType::Range) {
            std::regex reg("^[0-9]+,[0-9]+");

            if (!std::regex_match(iValue, reg))
                isValid = false;
        }

        if (!isValid)
            std::cout << "Bad value for argument: " << mID << std::endl;

        if (isValid)
            mValue = iValue;
    }

    std::pair<int, int>
    ArgumentParser::GetBounds()
    {
        assert(mType == Range);

        std::stringstream  stream(mValue);
        std::string strValue;
        std::vector<std::string> bounds;

        while( std::getline(stream, strValue, ','))
        {
            bounds.push_back(strValue);
        }

        assert(bounds.size() == 2);

        return {std::stoi(bounds[0]), std::stoi(bounds[1])};
    }

} // nCommon
