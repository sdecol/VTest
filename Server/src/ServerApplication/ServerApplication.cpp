#include "ServerApplication/ServerApplication.hpp"

#include <fstream>
#include <random>
#include <regex>

#define SERVER_TIMEOUT 10000

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

        mServer.Post("/client_connect", [this](const httplib::Request& iReq, httplib::Response& oRes)
        {
            AnswerClientConnection(iReq, oRes);
        });

        mServer.Post("/client_answer", [this](const httplib::Request& iReq, httplib::Response& oRes)
        {
            ProcessClientAnswer(iReq, oRes);
        });

        mServer.Post("/ping", [this](const httplib::Request& iReq, httplib::Response& oRes)
        {
            ProcessPing(iReq, oRes);
        });

        //Using a separate thread to check if clients are connected

        mPingThread = std::thread([this]()
                                {
                                    CheckClientTimeout();
                                });


        mServerThread = std::thread([this]{
            mServer.listen("0.0.0.0", mPort);
        });
    }

    void ServerApplication::Run()
    {
        std::string input;

        std::cout<<"Enter \"exit\" to close the server"<<std::endl;
        std::cin>>input;

        if(input == "exit")
            mIsRunning = false;
    }

    void ServerApplication::Quit()
    {
        mServer.stop();
        mServerThread.join();
        mPingThread.join();

        SaveGameHistory();
    }

    void ServerApplication::AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes)
    {
        mClientLock = false;
        //TODO: manage invalid json
        auto clientData = nlohmann::json::parse(iReq.body);
        const std::string& clientName = clientData["name"];
        int clientID = clientData["id"];
        bool isIA = clientData["is_ia"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(),
                                     [&clientName, &clientID](const ConnectedClient& iClient)
                                     {
                                         return iClient.mIsIA ? iClient.mID == clientID : iClient.mName == clientName;
                                     });

        if (clientIT != mClients.end())
        {
            std::cout << "Client already exists: " << clientName << std::endl;
            nlohmann::json answer;
            answer["connection_status"] = "failure";
            answer["message"] = "A client with the same name already exists";
            oRes.set_content(answer.dump(), "application/json");
        }
        else
        {

            //Check if the client has already been playing
            auto clientIT = std::find_if(mHistory.begin(), mHistory.end(), [clientName](const GameHistory& iEntry)
            {

                return iEntry.mPlayerName == clientName;
            });

            std::cout << (clientIT != mHistory.end() ? "Welcome back !" : "New client connected: ") << clientName
                      << std::endl;
            int newClientID = mCurrentClientID++;
            mClients.emplace_back(newClientID, clientName, GenerateRandomNumber(), isIA);
            nlohmann::json answer;
            answer["connection_status"] = "success";
            answer["client_id"] = newClientID;
            oRes.set_content(answer.dump(), "application/json");

            //We check if there's a pending game for this player (just in case the client closed and reopened before timeout
            auto pendingGameIT = std::find_if(mHistory.begin(), mHistory.end(), [clientName](const GameHistory& iEntry)
            {

                return iEntry.mPlayerName == clientName && iEntry.mGameState == GameHistory::GameState::Pending;
            });

            //If no pending game has been found, we add a new entry for this player
            if (pendingGameIT == mHistory.end())
            {
                GameHistory entry;
                entry.mPlayerName = clientName;
                entry.mBeginTime = std::chrono::system_clock::now();
                entry.mNbTry = 0;
                entry.mGameState = GameHistory::GameState::Pending;
                mHistory.emplace_back(entry);
            }
        }

        mClientLock = true;
    }

    void ServerApplication::ProcessClientAnswer(const httplib::Request& iReq, httplib::Response& oRes)
    {
        mClientLock = false;
        auto clientData = nlohmann::json::parse(iReq.body);

        const std::string& clientName = clientData["name"];
        int clientID = clientData["id"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(),
                                     [&clientName, &clientID](const ConnectedClient& iClient)
                                     {
                                         return iClient.mIsIA ? iClient.mID == clientID : iClient.mName == clientName;
                                     });

        if (clientIT == mClients.end())
        {
            std::cout << "Answer received from unknown client !" << std::endl;
            nlohmann::json jsonAnswer;
            jsonAnswer["server_answer_type"] = "invalid_client";
            oRes.set_content(jsonAnswer.dump(), "application/json");
            return;
        }

        const std::string& answer = clientData["number"];

        std::regex reg("^-*[0-9]+");

        nlohmann::json jsonAnswer;

        //Answer is not a valid number
        if (answer.empty() || !std::regex_match(answer, reg))
        {
            jsonAnswer["server_answer_type"] = "invalid_number";
        }
        else
        {
            clientIT->mScore++;

            GameHistory* history = nullptr;

            if (!clientIT->mIsIA)
            {
                history = GetPendingGame(clientIT->mName);
                assert(history != nullptr); //There should be a pending game registered !
            }

            if (history != nullptr)
                history->mNbTry = clientIT->mScore;

            jsonAnswer["server_answer_type"] = "number_check";
            int number = std::stoi(answer);
            if (number == clientIT->mRandomNumber)
            {
                jsonAnswer["server_answer"] = "success";
                jsonAnswer["score"] = std::to_string(clientIT->mScore);
                mClients.erase(clientIT);

                if (history != nullptr)
                    history->mGameState = GameHistory::GameState::Win;

            }
            else
            {
                if (mLimit >= 0 && clientIT->mScore == mLimit)
                {
                    jsonAnswer["server_answer_type"] = "limit_exceeded";
                    jsonAnswer["good_answer"] = std::to_string(clientIT->mRandomNumber);

                    if (history != nullptr)
                        history->mGameState = GameHistory::GameState::Loose;

                }
                else
                {
                    if (number > clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "lower";
                    else if (number < clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "upper";
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
        //TODO handle invalid json

        auto body = nlohmann::json::parse(iReq.body);

        const std::string& name = body["name"];
        int clientID = body["id"];

        auto it = std::find_if(mClients.begin(), mClients.end(), [&name, &clientID](const ConnectedClient& iClient)
        {
            return iClient.mIsIA ? iClient.mID == clientID : iClient.mName == name;
        });

        if (it == mClients.end())
            return;

        it->mLastMessageTime = std::chrono::system_clock::now();

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
                auto now = std::chrono::system_clock::now();

                auto it = std::find_if(mClients.begin(), mClients.end(), [now](const ConnectedClient& iClient)
                {
                    auto ellapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - iClient.mLastMessageTime);
                    return ellapsedTime.count() > SERVER_TIMEOUT;
                });

                if (it != mClients.end())
                {
                    std::cout << "Client timeout: " << (it->mIsIA ? std::to_string(it->mID) : it->mName) << std::endl;

                    if (!it->mIsIA)
                    {
                        GameHistory* history = GetPendingGame(it->mName);

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

    GameHistory* ServerApplication::GetPendingGame(const std::string& iPlayerName)
    {
        auto it = std::find_if(mHistory.begin(), mHistory.end(), [iPlayerName](const GameHistory& iGame)
        {

            return iGame.mPlayerName == iPlayerName && iGame.mGameState == GameHistory::GameState::Pending;

        });

        if (it == mHistory.end())
            return nullptr;

        return &(*it);
    }

    void ServerApplication::SaveGameHistory()
    {
        if(mHistory.empty())
            return;

        std::ofstream historyFile("game_history.json");

        nlohmann::json jsonArray = nlohmann::json::array();

        //To have automatic formated array
        for(const auto& history : mHistory)
            jsonArray.push_back(history.ToJson());

        if(historyFile.is_open())
        {
            historyFile << jsonArray.dump(4);
            historyFile.close();
        }
        else
            std::cerr<<"Can't write to history file!"<<std::endl;
    }

    void ServerApplication::LoadGameHistory()
    {
        std::ifstream historyFile("game_history.json");

        if(historyFile.is_open())
        {
            nlohmann::json array;
            try
            {
                historyFile >> array;

                if(array.is_array())
                {
                    for(const auto& jsonObj : array)
                    {
                        GameHistory history;
                        history.FromJson(jsonObj);
                        mHistory.push_back(history);
                    }
                }
                else
                {
                    std::cerr<<"json history data is not a valid array"<<std::endl;
                }
            }
            catch(const std::exception& e)
            {
                std::cerr<<"Failed to retrieve json data"<<std::endl;
            }

            historyFile.close();
        }
        else
            std::cerr<<"Can't read from the history file !"<<std::endl;
    }

} // nApplication
