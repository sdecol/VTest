#include "ConnectedClient.hpp"

namespace nApplication {

    ConnectedClient::ConnectedClient(const std::string &iIP, const std::string &iName, int iNumber) :
        mIP(iIP),
        mName(iName),
        mRandomNumber(iNumber),
        mScore(0)
    {

    }

} // nApplication