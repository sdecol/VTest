#pragma once

#include "ServerApplication/ConnectedClient.hpp"

#include "httplib/httplib.h"
#include "nlohmann/json.hpp"

#include <random>
#include <string>

namespace nApplication
{
    class ServerApplication
    {
    public:

        // Construction / Destruction
        ServerApplication( int iPort, int iLimit, int iMinValue, int iMaxValue);
        ~ServerApplication() = default;

        inline bool IsRunning() const noexcept { return mIsRunning; }

        // Initializes the server
        void Start();

        //Stops the server
        void Quit();

    private:

        //Checks that the new client is not registered.
        //If a client with the same name already exists, we notify the client in the answer.
        //If the client has not been registered yet and has a valid name, we add a new client to the inventory.
        void AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes);

        void ProcessClientAnswer(const httplib::Request& iReq, httplib::Response& oRes);

        void ProcessPing(const httplib::Request& iReq, httplib::Response& oRes);

        [[nodiscard]]
        int GenerateRandomNumber() const;

        void CheckClientTimeout();

    private:

        httplib::Server mServer;

        bool mIsRunning = false;

        int mPort = 0;
        int mLimit = -1;
        int mMinValue = 0;
        int mMaxValue = 0;

        std::vector<ConnectedClient> mClients;
    };

} //nApplication
