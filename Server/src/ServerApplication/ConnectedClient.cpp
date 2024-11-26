#include "ConnectedClient.hpp"

namespace nApplication {

    ConnectedClient::ConnectedClient(const std::string &iIP, const std::string &iName, int iNumber) :
        mIP(iIP),
        mName(iName),
        mRandomNumber(iNumber),
        mScore(0)
    {
        mLastMessageTime = std::chrono::system_clock::now();
    }

} // nApplication