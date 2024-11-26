#pragma once

#include "ClientApplication/ClientProgram.hpp"

namespace nApplication {

class HumanProgram: public ClientProgram {

public:

    HumanProgram() : ClientProgram(){}
    virtual ~HumanProgram() = default;

    std::string GetInput() override;

};

} // nApplication
