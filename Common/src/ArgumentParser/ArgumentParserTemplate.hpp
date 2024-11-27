#pragma once

#include "ArgumentParser.hpp"

namespace nCommon
{
    template<class T>
    class ArgumentParserTemplate : public ArgumentParser
    {
    public:

        ArgumentParserTemplate(const std::string& iID, const std::string& iDefaultValue) : ArgumentParser(iID,
                                                                                                          iDefaultValue)
        {
        }

        virtual ~ArgumentParserTemplate()
        {
        }

        [[nodiscard]]
        inline T Value() const noexcept
        {
            return mValue;
        }

    public:

        void ParseValue(const std::string& iValue) override
        {
        }

    protected:

        T mValue;
    };
}
