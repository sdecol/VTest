#pragma once

#include <string>

namespace nApplication
{
    /**
     * Base class for client inputs
     */
    class ClientInput
    {

    public:

        //Construction / Destruction

        ClientInput() = default;

        virtual ~ClientInput() = default;

        virtual std::string GetInput() = 0;

        virtual void GetLowerValue()
        {
        }

        virtual void GetHigherValue()
        {
        }

    };

} // nCommon
