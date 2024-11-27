#pragma once

#include "ClientApplication/ClientProgram.hpp"

namespace nApplication
{

    class IAProgram : public ClientProgram
    {

    public:

        IAProgram();

        virtual ~IAProgram() = default;

    public:

        std::string GetInput() override;

        void GetLowerValue() override;

        void GetUpperValue() override;

    private:

        int mCurrentValue = 0;
        int mMinValue = 0;
        int mMaxValue = 0;
    };

} // nApplication
