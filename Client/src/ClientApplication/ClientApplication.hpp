#pragma once

#include "httplib/httplib.h"
#include "nlohmann/json.hpp"

#include <memory>
#include <string>

#include "ClientApplication/ClientInput/ClientInput.hpp"

namespace nApplication
{

    class ClientInput;

    class ClientApplication
    {
    public:

        ClientApplication(const std::string& iClientName, const std::string& iHost, int iPort, bool iIAMode);

        ~ClientApplication();

    public:

        // Getters / setters

        [[nodiscard]]
        inline bool IsRunning() const noexcept
        {
            return mIsRunning;
        }

    public:

        //Connects to the server
        void Start();

        // Sends number to the server and process answers received
        void Run();

        //Ends ping thread and stops the client
        void Quit();

    private:

        //Prcess the answer sent by the server after having sent a number
        void ProcessServerAnswer(const nlohmann::json& iServerAnswer);

        //Sends a ping message to the server
        void SendPing();

        //Display history sent in json by the server
        void DisplayHistory(const nlohmann::json& iServerAnswer) const;

    private:

        int mClientID = -1; // Id of the client (used to identify IA players)
        std::string mName; // Name of the player
        std::string mHost; // Host used to connect to the server
        int mPort; // Port used to connect to the server
        std::atomic<bool> mIsRunning = false; // True as long as the client application is running
        bool mIAMode = false; // True if the player is an IA

        std::unique_ptr<httplib::Client> mClient = nullptr; //httplib component used to connect to the server and send requests
        ClientInput* mPlayerInput = nullptr; // Input used by the application to get the number to send to the server
        std::thread mPingThread; //Thread used to send ping request to the server to notify it that the client is still here
    };

} //nApplication