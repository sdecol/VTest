#pragma once

#include <chrono>
#include <string>

namespace nApplication {

class ConnectedClient
{

public:

    // Construction / Destruction

    ConnectedClient( int iID, const std::string& iName, int iNumber, bool iIA);
    ~ConnectedClient() = default;

public:

int mID = 0;
bool mIsIA = false;
std::string mName;
int mRandomNumber;
int mScore;
std::chrono::time_point<std::chrono::system_clock> mLastMessageTime;

};

} // nApplication
