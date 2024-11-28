#pragma once

#include "ArgumentParser/ArgumentParserTemplate.hpp"

namespace nCommon
{
    /**
     * Class representing a boolean argument parser
     */
    class BoolArgumentParser : public ArgumentParserTemplate<bool>
    {

    public:

        BoolArgumentParser(const std::string& iID, const std::string& iDefaultValue) : ArgumentParserTemplate<bool>(iID,
                                                                                                                    iDefaultValue)
        {
            mValue = false;
        }

        ~BoolArgumentParser() = default;

    public:

        void ParseValue(const std::string& iValue) override;

        void Activate() override;

    };

} // nCommon
