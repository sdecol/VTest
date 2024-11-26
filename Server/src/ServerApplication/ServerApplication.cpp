#include "ServerApplication/ServerApplication.hpp"

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
        mServer.Post("/client_connect", [this](const httplib::Request &iReq, httplib::Response &oRes)
        {
            AnswerClientConnection(iReq, oRes);
        });

        mServer.Post("/client_answer", [this](const httplib::Request &iReq, httplib::Response &oRes)
        {
            ProcessClientAnswer(iReq, oRes);
        });

        mServer.Post("/ping", [this](const httplib::Request &iReq, httplib::Response &oRes)
        {
            ProcessPing(iReq, oRes);
        });

        //Using a separate thread to check if clients are connected

        std::thread sleepThread([this]()
        {
            CheckClientTimeout();
        });


        mServer.listen("0.0.0.0", mPort);
        sleepThread.join();
    }

    void ServerApplication::Quit()
    {
        mServer.stop();
    }

    void ServerApplication::AnswerClientConnection(const httplib::Request &iReq, httplib::Response &oRes)
    {
        mClientLock = false;
        //TODO: manage invalid json
        auto clientData = nlohmann::json::parse(iReq.body);
        const std::string &clientName = clientData["name"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(), [clientName](const ConnectedClient &iClient)
        {
            return iClient.mName == clientName;
        });

        if (clientIT != mClients.end()) {
            std::cout << "Client already exists: " << clientName << std::endl;
            nlohmann::json answer;
            answer["connection_status"] = "failure";
            answer["message"] = "A client when the same name already exists";
            oRes.set_content(answer.dump(), "application/json");
        } else {
            std::cout << "New client connected: " << clientName << std::endl;
            mClients.emplace_back(iReq.remote_addr, clientName, GenerateRandomNumber());
            nlohmann::json answer;
            answer["connection_status"] = "success";
            oRes.set_content(answer.dump(), "application/json");
        }

        mClientLock = true;
    }

    void ServerApplication::ProcessClientAnswer(const httplib::Request &iReq, httplib::Response &oRes)
    {
        auto clientData = nlohmann::json::parse(iReq.body);

        const std::string &clientName = clientData["name"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(), [clientName](const ConnectedClient &iClient)
        {
            return iClient.mName == clientName;
        });

        if (clientIT == mClients.end()) {
            std::cout << "Answer received from unknown client !" << std::endl;
            nlohmann::json jsonAnswer;
            jsonAnswer["server_answer_type"] = "invalid_client";
            oRes.set_content(jsonAnswer.dump(), "application/json");
            return;
        }

        const std::string &answer = clientData["number"];

        std::regex reg("^-*[0-9]+");

        nlohmann::json jsonAnswer;

        //Answer is not a valid number
        if (answer.empty() || !std::regex_match(answer, reg)) {
            jsonAnswer["server_answer_type"] = "invalid_number";
        } else {
            clientIT->mScore++;

            jsonAnswer["server_answer_type"] = "number_check";
            int number = std::stoi(answer);
            if (number == clientIT->mRandomNumber) {
                jsonAnswer["server_answer"] = "success";
                jsonAnswer["score"] = std::to_string(clientIT->mScore);
                mClients.erase(clientIT);
            } else {
                if (mLimit >= 0 && clientIT->mScore == mLimit) {
                    jsonAnswer["server_answer_type"] = "limit_exceeded";
                    jsonAnswer["good_answer"] = std::to_string(clientIT->mRandomNumber);
                } else {
                    if (number > clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "lower";
                    else if (number < clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "upper";
                }
            }
        }

        oRes.set_content(jsonAnswer.dump(), "application/json");
    }

    void ServerApplication::ProcessPing(const httplib::Request &iReq, httplib::Response &oRes)
    {
        if(!mClientLock)
            return;
        //TODO handle invalid json

        auto body = nlohmann::json::parse(iReq.body);

        const std::string &name = body["name"];

        auto it = std::find_if(mClients.begin(), mClients.end(), [name](const ConnectedClient &iClient)
        {
            return iClient.mName == name;
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
        while (mIsRunning) {
            if(mClientLock)
            {
                auto now = std::chrono::system_clock::now();

                auto it = std::find_if(mClients.begin(), mClients.end(), [now](const ConnectedClient &iClient)
                {
                    auto ellapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - iClient.mLastMessageTime);
                    return ellapsedTime.count() > SERVER_TIMEOUT;
                });

                if (it != mClients.end()) {
                    std::cout << "Client timeout: " << it->mName << std::endl;
                    mClients.erase(it);
                }
            }
        }
    }

} // nApplication
