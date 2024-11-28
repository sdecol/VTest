#pragma once

#include "ArgumentParser.hpp"

namespace nCommon
{
    /**
     * A template class with a template type representing the type of valud the corresponding argument should have
     */
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

        T mValue; // The value provided by the user (or parsed from the default value if the value sent by the user if not valid)
    };
}
