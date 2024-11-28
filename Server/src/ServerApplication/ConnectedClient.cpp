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

    bool ConnectedClient::operator==(const ConnectedClient& iClient) const
    {
        return iClient.mID == mID && iClient.mName == mName && iClient.mIP == mIP && iClient.mRandomNumber == mRandomNumber && iClient.mScore == mScore && iClient.mIsIA == mIsIA;
    }


} // nApplication