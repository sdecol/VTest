#pragma once

#include "ClientApplication/ClientInput/ClientInput.hpp"

namespace nApplication
{
    /**
     * Class used to get input from IA
     */
    class IAInput : public ClientInput
    {

    public:

        IAInput();

        virtual ~IAInput() = default;

    public:

        std::string GetInput() override;

        void GetLowerValue() override;

        void GetHigherValue() override;

    private:

        int mCurrentValue = 0; //Last sent number
        int mMinValue = 0; // Current value guessed as min value by the IA
        int mMaxValue = 0; // Current value guessed as max value by the IA
    };

} // nApplication
