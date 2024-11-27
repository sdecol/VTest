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

        //Finds the connected client with given id, name and ip. Returns nullptr if not found
        ConnectedClient* GetClient(int iID, const std::string& iPlayerName, const std::string& iClientIP);

        //Checks that the new client is not registered.
        //If a client with the same name already exists, we notify the client in the answer.
        //If the client has not been registered yet and has a valid name, we add a new client to the inventory.
        void AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes);

        void ProcessClientAnswer(const httplib::Request& iReq, httplib::Response& oRes);

        void ProcessPing(const httplib::Request& iReq, httplib::Response& oRes);

        [[nodiscard]]
        int GenerateRandomNumber() const;

        //Checks if any client is disconnected from the last time we received a ping.
        //If so, we consider the client from having given up and remove it from the list.
        void CheckClientTimeout();

        GameHistory* GetPendingGame(const std::string& iPlayerName, const std::string& iPlayerIP);

        //Saves the game history in a "history.json" file next to the executable of the server
        void SaveGameHistory();

        //Loads the history of the server from the "history.json" file and rebuilds the history list from it
        void LoadGameHistory();

        //Returns the history in json format for the given client and sort it by score
        //Keeps only the first best games according to the defined limit
        std::vector<nlohmann::json> GetHistoryForClient(const ConnectedClient* iClient);

        //Writes the history for the given client into the server answer
        void WriteJSonHistory(const ConnectedClient* iClient, nlohmann::json& oServerAnswer);

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
