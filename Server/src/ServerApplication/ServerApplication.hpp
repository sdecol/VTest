#pragma once

#include "ServerApplication/ConnectedClient.hpp"
#include "ServerApplication/GameHistory.hpp"

#include "httplib/httplib.h"
#include "nlohmann/json.hpp"

#include <random>
#include <string>

namespace nApplication
{
    /**
     * The entry point of the server application
     */
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

        //Removes the given client from the list
        void RemoveClient(const ConnectedClient& iClient);

        //Checks that the new client is not registered.
        //If a client with the same name already exists, we notify the client in the answer.
        //If the client has not been registered yet and has a valid name, we add a new client to the inventory.
        void AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes);

        //Checks that the answer received from the client to guess the random numer is valid.
        //If so, we check if the number corresponds to the random number to find.
        //If not, we answer to the client according to the value he sent.
        void ProcessClientAnswer(const httplib::Request& iReq, httplib::Response& oRes);

        //Answers a ping request received from a client to check if the client if always connected
        void ProcessPing(const httplib::Request& iReq, httplib::Response& oRes);

        //Generates a random number between the bounds of the server
        [[nodiscard]]
        int GenerateRandomNumber() const;

        //Checks if any client is disconnected from the last time we received a ping.
        //If so, we consider the client from having given up and remove it from the list.
        void CheckClientTimeout();

        //Returns the current pending game for the given client name and ip
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

        std::thread mServerThread; // The main thread of the server used to answer requests from clients
        std::thread mPingThread; // Thread used to manage ping requests from clients

        bool mIsRunning = false;

        int mPort = 0; // Port number used to connect
        int mLimit = -1; // Limits of try (-1 for unlimited)
        int mMinValue = 0; // Min value used to generate random number
        int mMaxValue = 0; // Max value used to generate random number
        int mCurrentClientID = 0; // ID generated by the server for each new client (used to identify IA players)

        std::vector<ConnectedClient> mClients; // Currently connected clients
        std::atomic_bool mClientLock = true; // Used to prevent modifications on client list from ping thread when the server is using it.
        std::vector<GameHistory> mHistory; // Game history loaded from the history file.
    };

} //nApplication
