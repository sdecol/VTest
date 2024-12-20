#pragma once

#include "ArgumentParser/ArgumentParserTemplate.hpp"

namespace nCommon
{
    /**
     * A class representing a bounds argument parser with a min and max
     */
    class BoundsArgumentParser : public nCommon::ArgumentParserTemplate<std::pair<int, int> >
    {
    public:

        // Construction / Destruction

        BoundsArgumentParser(const std::string& iID, const std::string& iDefaultValue);

        virtual ~BoundsArgumentParser() = default;

    public:

        void ParseValue(const std::string& iValue) override;

    private:

        std::pair<int, int> ParseBounds(const std::string& iValue);
    };
} //nCommon
