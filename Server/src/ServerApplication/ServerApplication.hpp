#pragma once

#include "ServerApplication/ConnectedClient.hpp"
#include "ServerApplication/GameHistory.hpp"

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
        ServerApplication(int iPort, int iLimit, int iMinValue, int iMaxValue);

        ~ServerApplication() = default;

        inline bool IsRunning() const noexcept
        {
            return mIsRunning;
        }

        // Initializes the server
        void Start();

        //Waits for the server to be closed with a keyboard input
        void Run();

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

        GameHistory* GetPendingGame(const std::string& iPlayerName);

        //Saves the game history in a "history.json" file next to the executable of the server
        void SaveGameHistory();

        //Loads the history of the server from the "history.json" file and rebuilds the history list from it
        void LoadGameHistory();

    private:

        httplib::Server mServer;
        std::thread mServerThread;
        std::thread mPingThread;

        bool mIsRunning = false;

        int mPort = 0;
        int mLimit = -1;
        int mMinValue = 0;
        int mMaxValue = 0;
        int mCurrentClientID = 0;

        std::vector<ConnectedClient> mClients;
        std::atomic_bool mClientLock = true;
        std::vector<GameHistory> mHistory;
    };

} //nApplication
