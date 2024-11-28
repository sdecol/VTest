#include "ClientApplication/ClientInput/HumanInput.hpp"

#include <iostream>

namespace nApplication
{

    std::string HumanInput::GetInput()
    {
        std::string number;
        std::cout << "Enter a number" << std::endl;
        std::cin >> number;

        return number;
    }


} // nApplication