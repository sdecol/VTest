#pragma once

#include "ClientApplication/ClientInput/ClientInput.hpp"

namespace nApplication
{
    /**
     * Class used to get input from human player
     */
    class HumanInput : public ClientInput
    {

    public:

        HumanInput() : ClientInput()
        {
        }

        virtual ~HumanInput() = default;

        std::string GetInput() override;

    };

} // nApplication
