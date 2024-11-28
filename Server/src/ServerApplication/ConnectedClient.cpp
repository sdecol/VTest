#include "ConnectedClient.hpp"

namespace nApplication
{

    ConnectedClient::ConnectedClient(int iID, const std::string& iName, const std::string& iIP, int iNumber, bool iIA) :
            mID(iID),
            mName(iName),
            mIP(iIP),
            mRandomNumber(iNumber),
            mScore(0),
            mIsIA(iIA)
    {
        mLastPingTime = std::chrono::system_clock::now();
    }

} // nApplication