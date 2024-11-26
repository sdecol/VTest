#pragma once

#include <string>

namespace nApplication
{

class ClientProgram {

    public:

    //Construction / Destruction

    ClientProgram() = default;
    virtual ~ClientProgram() = default;

    virtual std::string GetInput() = 0;

    virtual void GetLowerValue(){}
    virtual void GetUpperValue(){}

};

} // nCommon
