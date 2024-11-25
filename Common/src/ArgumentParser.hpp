#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

namespace nCommon
{

class ArgumentParser
{

public:

    enum ArgumentType
    {
        Number,
        Range,
        String
    };

    // Construction / Destruction

    explicit ArgumentParser( const std::string & iID, ArgumentType iType, const std::string& iDefaultValue);
    ~ArgumentParser() = default;

    // Arguments validation
    inline void AddValidArguments(const std::vector<std::string>& iArguments) noexcept { mValidArguments.insert(mValidArguments.end(), iArguments.begin(), iArguments.end());}

    //Checks if the given argument is a valid one for this parser
    [[nodiscard]]
    inline bool IsValidArgument(const std::string& iArgument) const noexcept { return std::find(mValidArguments.begin(), mValidArguments.end(), iArgument) != mValidArguments.end();}

    //Checks if the given value is valid and assign it to mValue if so
    void ParseValue(const std::string& iValue);

    [[nodiscard]]
    inline std::string Value() const noexcept { return mValue; }

    //Asserts that this argument can be converted into a int and returns the converted value
    [[nodiscard]]
    inline int ToInt() const { assert(mType == Number); return mValue.empty() ? std::stoi(mDefaultValue) : std::stoi(mValue); }

    [[nodiscard]]
    std::pair<int,int> GetBounds();

private:

    std::string mID;
    ArgumentType mType;
    std::vector<std::string> mValidArguments;
    std::string mValue;
    std::string mDefaultValue;

};

} //nCommon