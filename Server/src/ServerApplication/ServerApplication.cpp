#include "ServerApplication/ServerApplication.hpp"

#include <random>

namespace nApplication
{
    ServerApplication::ServerApplication(int iPort, int iLimit, int iMinValue, int iMaxValue) :
            mPort(iPort),
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
        mServer.Post("/client_connect", [this](const httplib::Request& iReq, httplib::Response& oRes) {
                AnswerClientConnection(iReq, oRes);
        });

        mServer.Post("/client_answer", [this](const httplib::Request& iReq, httplib::Response& oRes)
        {
            ProcessClientAnswer(iReq, oRes);
        });

        mServer.listen("0.0.0.0", mPort);
    }

    void ServerApplication::Quit()
    {
        mServer.stop();
    }

    void ServerApplication::AnswerClientConnection(const httplib::Request& iReq, httplib::Response& oRes)
    {
        //TODO: manage invalid json
        auto clientData = nlohmann::json::parse(iReq.body);
        const std::string& clientName = clientData["name"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(), [clientName](const ConnectedClient& iClient)
        {
            return iClient.mName == clientName;
        });

        if(clientIT != mClients.end()) {

            std::cout<<"Client already exists: "<<clientName<<std::endl;
            nlohmann::json answer;
            answer["connection_status"] = "failure";
            answer["message"]="A client when the same name already exists";
            oRes.set_content(answer.dump(), "application/json");
        }
        else {
            std::cout<<"New client connected: "<<clientName<<std::endl;
            mClients.emplace_back(iReq.remote_addr, clientName, GenerateRandomNumber());
            nlohmann::json answer;
            answer["connection_status"] = "success";
            oRes.set_content(answer.dump(), "application/json");
        }
    }

    void ServerApplication::ProcessClientAnswer(const httplib::Request &iReq, httplib::Response &oRes)
    {
        auto clientData = nlohmann::json::parse(iReq.body);

        const std::string& clientName = clientData["name"];

        auto clientIT = std::find_if(mClients.begin(), mClients.end(), [clientName](const ConnectedClient& iClient)
        {
            return iClient.mName == clientName;
        });

        if(clientIT == mClients.end()) {
            std::cout<<"Answer received from unknown client !"<<std::endl;
            nlohmann::json jsonAnswer;
            jsonAnswer["server_answer_type"] = "invalid_client";
            oRes.set_content(jsonAnswer.dump(), "application/json");
            return;
        }

        const std::string& answer = clientData["number"];

        auto it = answer.begin();

        while(it != answer.end() && std::isdigit(*it)) ++it;

        nlohmann::json jsonAnswer;

        //Answer is not a valid number
        if(answer.empty() || it != answer.end()) {
            jsonAnswer["server_answer_type"] = "invalid_number";
        }
        else {
            clientIT->mScore++;

            jsonAnswer["server_answer_type"] = "number_check";
            int number = std::stoi(answer);
            if(number == clientIT->mRandomNumber) {
                jsonAnswer["server_answer"] = "success";
                jsonAnswer["score"] = std::to_string(clientIT->mScore);
                mClients.erase(clientIT);
            }
            else {
                if(mLimit >= 0 && clientIT->mScore == mLimit)
                {
                    jsonAnswer["server_answer_type"] = "limit_exceeded";
                    jsonAnswer["good_answer"] = std::to_string(clientIT->mRandomNumber);
                }
                else {
                    if(number > clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "lower";
                    else if(number < clientIT->mRandomNumber)
                        jsonAnswer["server_answer"] = "upper";
                }

            }
        }

        oRes.set_content(jsonAnswer.dump(), "application/json");
    }

    int ServerApplication::GenerateRandomNumber() const
    {
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int> dist(mMinValue, mMaxValue);

        return dist(engine);
    }



} // nApplication
