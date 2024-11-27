#include "ConnectedClient.hpp"

namespace nApplication
{

    ConnectedClient::ConnectedClient(int iID, const std::string& iName, int iNumber, bool iIA) :
            mID(iID),
            mName(iName),
            mRandomNumber(iNumber),
            mScore(0),
            mIsIA(iIA)
    {
        mLastMessageTime = std::chrono::system_clock::now();
    }

} // nApplication