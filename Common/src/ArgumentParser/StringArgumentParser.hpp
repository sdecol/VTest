#pragma once

#include "ArgumentParser/ArgumentParserTemplate.hpp"

namespace nCommon {

class StringArgumentParser : public ArgumentParserTemplate<std::string>
{
public:

    // Construction / Destruction

    StringArgumentParser( const std::string & iID, const std::string& iDefaultValue) : ArgumentParserTemplate<std::string>(iID, iDefaultValue){ mValue = mDefaultValue; }
    virtual ~StringArgumentParser() = default;

public:

    void ParseValue(const std::string &iValue) override;
};

} // nCommon
