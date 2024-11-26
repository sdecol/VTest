#include "ClientApplication/IAProgram.hpp"

#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <thread>

namespace nApplication {

    using namespace std::chrono_literals;

    IAProgram::IAProgram() : ClientProgram()
    {
        mMinValue = std::numeric_limits<int>::min();
        mMaxValue = std::numeric_limits<int>::max();
    }


    std::string IAProgram::GetInput()
    {
        std::this_thread::sleep_for(1000ms);
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int> dist(mMinValue, mMaxValue);

        mCurrentValue = dist(engine);
        std::cout<<"IA sending number: "<<mCurrentValue<<std::endl;
        return std::to_string(mCurrentValue);
    }

    void IAProgram::GetLowerValue()
    {
        mMaxValue = mCurrentValue;
    }

    void IAProgram::GetUpperValue()
    {
        mMinValue = mCurrentValue;
    }




} // nApplication