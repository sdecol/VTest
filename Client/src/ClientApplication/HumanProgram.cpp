#include "ClientApplication/HumanProgram.hpp"

#include <iostream>

namespace nApplication
{

    std::string HumanProgram::GetInput()
    {
        std::string number;
        std::cout << "Enter a number" << std::endl;
        std::cin >> number;

        return number;
    }


} // nApplication