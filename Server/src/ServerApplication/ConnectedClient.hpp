#pragma once

#include <chrono>
#include <string>

namespace nApplication
{
    /**
     * A class used to store the data of a connected client
     */
    class ConnectedClient
    {

    public:

        // Construction / Destruction

        ConnectedClient(int iID, const std::string& iName, const std::string& iIP, int iNumber, bool iIA);

        ~ConnectedClient() = default;

        bool operator==(const ConnectedClient& iClient) const;
    public:

        int mID = 0; // Generated if from the server
        std::string mIP; // The ip of the client retrieved from http request
        bool mIsIA = false; // True if the client is an IA
        std::string mName; // The name of the player
        int mRandomNumber; // The random number generated for this player (for the current game)
        int mScore; // The current score of the player
        std::chrono::time_point<std::chrono::system_clock> mLastPingTime; // Last time we received a ping request from this client.

    };

} // nApplication
