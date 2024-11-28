#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace nCommon
{
    /**
     * Base class for argument parsers
     */
    class ArgumentParser
    {
    public:

        // Construction / Destruction

        ArgumentParser(const std::string& iID, const std::string& iDefaultValue) :
                mID(iID),
                mDefaultValue(iDefaultValue)
        {

        };

        virtual ~ArgumentParser() = default;

        // Adds one or many argument ids for this parser
        inline void AddValidArguments(const std::vector<std::string>& iArguments) noexcept
        {
            mValidArguments.insert(mValidArguments.end(), iArguments.begin(), iArguments.end());
        }

        //Checks if the given argument is a valid one for this parser
        [[nodiscard]]
        inline bool IsValidArgument(const std::string& iArgument) const noexcept
        {
            return std::find(mValidArguments.begin(), mValidArguments.end(), iArgument) != mValidArguments.end();
        }

        //Checks if the given value is valid and assign it to mValue if so
        virtual void ParseValue(const std::string& iValue) = 0;

        //Should be called if the corresponding argument has been sent by the user
        virtual void Activate()
        {
        }

    protected:

        std::string mID; //General id for this parser
        std::vector<std::string> mValidArguments; // Valid arguments for this parser (ex : "-n", --name")
        std::string mDefaultValue; // Default value used if the user provided a non valid one

    };

} //nCommon