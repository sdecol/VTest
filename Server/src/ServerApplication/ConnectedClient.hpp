#pragma once

#include <chrono>
#include <string>

namespace nApplication {

class ConnectedClient
{

public:

    // Construction / Destruction

    ConnectedClient( const std::string& iIP, const std::string& iName, int iNumber);
    ~ConnectedClient() = default;

public:

std::string mIP;
std::string mName;
int mRandomNumber;
int mScore;
std::chrono::time_point<std::chrono::system_clock> mLastMessageTime;

};

} // nApplication
