#include "ArgumentParser/ArgumentParserTemplate.hpp"

namespace nCommon
{
    class IntArgumentParser : public ArgumentParserTemplate<int>
    {

    public:

        // Construction / Destruction

        IntArgumentParser(const std::string& iID, const std::string& iDefaultValue) : ArgumentParserTemplate<int>(iID,
                                                                                                                  iDefaultValue)
        {
            mValue = std::stoi(mDefaultValue);
        }

        virtual ~IntArgumentParser() = default;

    public:

        void ParseValue(const std::string& iValue) override;

    };
} // nCommon
