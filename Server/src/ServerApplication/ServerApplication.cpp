#include "ServerApplication/ServerApplication.hpp"

#include <fstream>
#include <random>
#include <regex>

#define SERVER_TIMEOUT 10000
#define BEST_SCORE_COUNT 5

namespace nApplication
{
    ServerApplication::ServerApplication(int iPort, int iLimit, int iMinValue, int iMaxValue) : mPort(iPort),
        mLimit(iLimit),
        mMinValue(iMinValue),
        mMaxValue(iMaxValue)
    {
        mMinValue = std::max<int>(mMinValue, 0);
        mMaxValue = std::max<int>(mMaxValue, 0);

        if (mMinValue > mMaxValue)
            std::swap(mMinValue, mMaxValue);
    }

    void
    ServerApplication::Start()
    {
        mIsRunning = true;

        LoadGameHistory();

        mServer.Post("/client_connect", [this](const auto& iReq, auto& oRes)
        {
            AnswerClientConnection(iReq, oRes);
        });

        mServer.Post("/client_answer", [this](const auto& iReq, auto& oRes)
        {
            ProcessClientAnswer(iReq, oRes);
        });

        mServer.Post("/ping", [this](const auto& iReq, auto& oRes)
        {
            ProcessPing(iReq, oRes);
        });

        //Using a separate thread to check if clients are connected

        mPingThread = std::thread([this]()
        {
            CheckClientTimeout();
        });


        mServerThread = std::thread([this]
        {
            mServer.listen("0.0.0.0", mPort);
        });
    }

    void ServerApplication::Run()
    {
        std::string input;

        std::cout << "Enter \"exit\" to close the server" << std::endl;
        std::cin >> input;

        if (input == "exit")
            mIsRunning = false;
    }

    void ServerApplication::Quit()
    {
        mServer.stop();

        if(mServerThread.joinable())
            mServerThread.join();

        if(mPingThread.joinable())
            mPingThread.join();

        SaveGameHistory();
    }

    ConnectedClient*
    ServerApplication::GetClient(int iID, const std::string& iPlayerName, const std::string& iClientIP)
    {
        auto it = std::find_if(mClients.begin(), mClients.end(), [iID, iPlayerName, iClientIP](const auto& iClient)
        {
            return iClient.mIsIA ? iClient.mID == iID : iClient.mName == iPlayerName && iClient.mIP == iClientIP;
        });

        return it == mClients.end() ? nullptr : &(*it);
    }

    void ServerApplication::AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes)
    {
        mClientLock = false;

        nlohmann::json clientData;

        //Check if we can read the json data from the request
        try
        {
            clientData = nlohmann::json::parse(iReq.body);
        } catch (const std::exception& e)
        {
            std::cerr << "Failed to read request body" << std::endl;
            oRes.set_content("INVALID_JSON", "text/plain");
            oRes.status = 400;
            return;
        }

        const std::string& clientName = clientData["name"];
        int clientID = clientData["id"];
        const std::string& clientIP = iReq.remote_addr;
        bool isIA = clientData["is_ia"];

        auto* connectedClient = GetClient(clientID, clientName, clientIP);

        if (connectedClient != nullptr)
        {
            std::cout << "Client already exists: " << clientName << std::endl;
            nlohmann::json answer;
            answer["connection_status"] = "failure";
            answer["message"] = "A client with the same name and ip already exists";
            oRes.set_content(answer.dump(), "application/json");
        } else
        {
            //Check if the client has already been playing
            auto clientIT = std::find_if(mHistory.begin(), mHistory.end(), [&clientName, &clientIP](const auto& iEntry)
            {
                return iEntry.mPlayerName == clientName && iEntry.mPlayerIP == clientIP;
            });

            std::cout << (clientIT != mHistory.end() ? "Welcome back !" : "New client connected: ") << clientName <<
                    std::endl;
            int newClientID = mCurrentClientID++;
            mClients.emplace_back(newClientID, clientName, iReq.remote_addr, GenerateRandomNumber(), isIA);

            nlohmann::json answer;
            answer["connection_status"] = "success";
            answer["client_id"] = newClientID;
            oRes.set_content(answer.dump(), "application/json");

            //We check if there's a pending game for this player (just in case the client closed and reopened before timeout
            auto* pendingGame = GetPendingGame(clientName, clientIP);

            //If no pending game has been found, we add a new entry for this game
            if (pendingGame == nullptr)
            {
                GameHistory entry;
                entry.mPlayerName = clientName;
                entry.mPlayerIP = iReq.remote_addr;
                entry.RecordBeginTime();
                entry.mScore = 0;
                entry.mGameState = GameHistory::GameState::Pending;
                mHistory.emplace_back(entry);
            }
        }

        mClientLock = true;
    }

    void ServerApplication::ProcessClientAnswer(const httplib::Request& iReq, httplib::Response& oRes)
    {
        mClientLock = false;

        nlohmann::json clientData;

        //We check if we have a valid json data in the request body
        try
        {
            clientData = nlohmann::json::parse(iReq.body);
        }
        catch(const std::exception& e)
        {
            oRes.set_content("INVALID_JSON", "text/plain");
            oRes.status = 400;
            return;
        }

        //we check if the client who answered is a registered one.
        //If not, we simply ignore it
        const std::string& clientName = clientData["name"];
        const std::string& clientIP = iReq.remote_addr;
        int clientID = clientData["id"];

        auto* connectedClient = GetClient(clientID, clientName, clientIP);

        if (connectedClient == nullptr)
        {
            std::cerr << "Answer received from unknown client !" << std::endl;
            nlohmann::json jsonAnswer;
            jsonAnswer["server_answer_type"] = "invalid_client";
            oRes.set_content(jsonAnswer.dump(), "application/json");
            return;
        }

        const std::string& answer = clientData["number"];

        bool validNumber = true;

        int number = 0;

        try
        {
            number = std::stoi(answer);
        }
        catch(std::exception& e)
        {
            validNumber = false;
        }

        nlohmann::json jsonAnswer;

        //Answer is not a valid number
        if (!validNumber)
        {
            jsonAnswer["server_answer_type"] = "invalid_number";
        } else
        {
            connectedClient->mScore++;

            GameHistory* history = nullptr;

            if (!connectedClient->mIsIA)
            {
                history = GetPendingGame(connectedClient->mName, connectedClient->mIP);
                assert(history != nullptr); //There should be a pending game registered !
            }

            if (history != nullptr)
                history->mScore = connectedClient->mScore;

            jsonAnswer["server_answer_type"] = "number_check";

            //The client found the random number
            //We send him the response and his score, and his 5 best scores in json format
            if (number == connectedClient->mRandomNumber)
            {
                jsonAnswer["server_answer"] = "success";
                jsonAnswer["score"] = std::to_string(connectedClient->mScore);

                auto it = std::find_if(mClients.begin(), mClients.end(),
                                       [clientID, &clientName, &clientIP](const auto& iClient)
                                       {
                                           return iClient.mIsIA
                                                      ? iClient.mID == clientID
                                                      : iClient.mName == clientName && iClient.mIP == clientIP;
                                       });

                if (it != mClients.end())
                    mClients.erase(it);

                if (history != nullptr)
                {
                    history->mGameState = GameHistory::GameState::Win;
                    history->RecordEndTime();
                    WriteJSonHistory(connectedClient, jsonAnswer);
                }
            } else
            {
                //The client provided a bad answer, we send him a response to tell if the number to find is higher or lower
                if (mLimit >= 0 && connectedClient->mScore == mLimit)
                {
                    jsonAnswer["server_answer_type"] = "limit_exceeded";
                    jsonAnswer["good_answer"] = std::to_string(connectedClient->mRandomNumber);

                    if (history != nullptr)
                    {
                        history->mGameState = GameHistory::GameState::Loose;
                        WriteJSonHistory(connectedClient, jsonAnswer);
                    }
                } else
                {
                    if (number > connectedClient->mRandomNumber)
                        jsonAnswer["server_answer"] = "lower";
                    else if (number < connectedClient->mRandomNumber)
                        jsonAnswer["server_answer"] = "higher";
                }
            }
        }

        oRes.set_content(jsonAnswer.dump(), "application/json");
        mClientLock = true;
    }

    void ServerApplication::ProcessPing(const httplib::Request& iReq, httplib::Response& oRes)
    {
        if (!mClientLock)
            return;

        nlohmann::json body;

        //We check if the data retrieved from the request is valid
        try
        {
            body = nlohmann::json::parse(iReq.body);
        } catch (const std::exception& e)
        {
            oRes.status = 400;
            std::cerr << "Failed to read request body" << std::endl;
            oRes.set_content("invalid_json", "tex/plain");
            return;
        }

        //We read the client data and update the last ping time

        const std::string& clientName = body["name"];
        int clientID = body["id"];
        const std::string& clientIP = iReq.remote_addr;

        auto* connectedClient = GetClient(clientID, clientName, clientIP);

        if (connectedClient == nullptr)
            return;

        connectedClient->mLastPingTime = std::chrono::system_clock::now();

        oRes.set_content("ping_received", "text/plain");
    }


    int ServerApplication::GenerateRandomNumber() const
    {
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int> dist(mMinValue, mMaxValue);

        return dist(engine);
    }

    void ServerApplication::CheckClientTimeout()
    {
        while (mIsRunning)
        {
            if (mClientLock)
            {
                //We check if any client has not been responding for a while,
                //If so, we remove it from the list
                auto now = std::chrono::system_clock::now();

                auto it = std::find_if(mClients.begin(), mClients.end(), [now](const auto& iClient)
                {
                    auto ellapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - iClient.mLastPingTime);
                    return ellapsedTime.count() > SERVER_TIMEOUT;
                });

                //We print a message to tell which client has been disconnected
                if (it != mClients.end())
                {
                    std::cout << "Client timeout: " << (it->mIsIA ? std::to_string(it->mID) : it->mName) << std::endl;

                    if (!it->mIsIA)
                    {
                        GameHistory* history = GetPendingGame(it->mName, it->mIP);

                        if (history != nullptr)
                        {
                            history->mGameState = GameHistory::GameState::GaveUp;
                            history->RecordEndTime();
                        }
                    }

                    mClients.erase(it);
                }
            }
        }
    }

    GameHistory* ServerApplication::GetPendingGame(const std::string& iPlayerName, const std::string& iPlayerIP)
    {
        auto it = std::find_if(mHistory.begin(), mHistory.end(), [&iPlayerName, &iPlayerIP](const auto& iGame)
        {
            return iGame.mPlayerName == iPlayerName && iGame.mPlayerIP == iPlayerIP &&
                   iGame.mGameState == GameHistory::GameState::Pending;
        });

        if (it == mHistory.end())
            return nullptr;

        return &(*it);
    }

    void ServerApplication::SaveGameHistory()
    {
        if (mHistory.empty())
            return;

        std::ofstream historyFile("game_history.json");

        //We try to open the historyFile and write all game histories inside.
        if (historyFile.is_open())
        {
            nlohmann::json jsonArray = nlohmann::json::array();

            //To have automatic formated array
            for (const auto& history: mHistory)
                jsonArray.push_back(history.ToJson());

            historyFile << jsonArray.dump(4);
            historyFile.close();
        } else
            std::cerr << "Can't write to history file!" << std::endl;
    }

    void ServerApplication::LoadGameHistory()
    {
        std::ifstream historyFile("game_history.json");

        if (historyFile.is_open())
        {
            nlohmann::json array;
            try
            {
                historyFile >> array;

                if (array.is_array())
                {
                    for (const auto& jsonObj: array)
                    {
                        GameHistory history;
                        history.FromJson(jsonObj);
                        mHistory.push_back(history);
                    }
                } else
                {
                    std::cerr << "json history data is not a valid array" << std::endl;
                }
            } catch (const std::exception& e)
            {
                std::cerr << "Failed to retrieve json data" << std::endl;
            }

            historyFile.close();
        }
    }

    std::vector<nlohmann::json> ServerApplication::GetHistoryForClient(const nApplication::ConnectedClient* iClient)
    {
        assert(iClient != nullptr);
        std::vector<nlohmann::json> result;
        std::vector<GameHistory> clientHistory;

        //We get the game history for the given client and then, we sort it according to scores

        std::copy_if(mHistory.begin(), mHistory.end(), std::back_inserter(clientHistory),
                     [iClient](const auto& iHistory)
                     {
                         return iHistory.mPlayerName == iClient->mName && iHistory.mPlayerIP == iClient->mIP;
                     });

        std::sort(clientHistory.begin(), clientHistory.end(), [](const auto& iFirst, const auto& iSecond)
        {
            return iFirst.mScore < iSecond.mScore;
        });

        //We return only the BEST_SCORE_COUNT histories in json format
        for (size_t i = 0; i < clientHistory.size() && i < BEST_SCORE_COUNT; i++)
            result.emplace_back(clientHistory[i].ToJson());

        return result;
    }

    void ServerApplication::WriteJSonHistory(const nApplication::ConnectedClient* iClient,
                                             nlohmann::json& oServerAnswer)
    {
        //We get game histories corresponding to the best scores and write it into the response in json format
        auto clientHistory = GetHistoryForClient(iClient);

        nlohmann::json array;

        for (auto& history: clientHistory)
            array.emplace_back(history);

        oServerAnswer["history"] = array;
    }
} // nApplication
