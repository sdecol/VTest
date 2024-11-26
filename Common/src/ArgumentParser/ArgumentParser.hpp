#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace nCommon
{

class ArgumentParser
{
public:

    // Construction / Destruction

    ArgumentParser( const std::string & iID, const std::string& iDefaultValue) :
        mID(iID),
        mDefaultValue(iDefaultValue)
    {

    };
    virtual ~ArgumentParser() = default;

    // Arguments validation
    inline void AddValidArguments(const std::vector<std::string>& iArguments) noexcept { mValidArguments.insert(mValidArguments.end(), iArguments.begin(), iArguments.end());}

    //Checks if the given argument is a valid one for this parser
    [[nodiscard]]
    inline bool IsValidArgument(const std::string& iArgument) const noexcept { return std::find(mValidArguments.begin(), mValidArguments.end(), iArgument) != mValidArguments.end();}

    //Checks if the given value is valid and assign it to mValue if so
    virtual void ParseValue(const std::string& iValue) = 0;

    //Should be called if the corresponding argument has been sent by the user
    virtual void Activate() {}

protected:

    std::string mID;
    std::vector<std::string> mValidArguments;
    std::string mDefaultValue;

};

} //nCommon