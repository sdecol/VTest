#include "ClientApplication/ClientApplication.hpp"

#include "ClientApplication/HumanProgram.hpp"
#include "ClientApplication/IAProgram.hpp"

#include "nlohmann/json.hpp"

namespace nApplication
{

    ClientApplication::ClientApplication(const std::string &iClientName, const std::string &iHost, int iPort, bool iIAMode) :
        mName(iClientName),
        mHost(iHost),
        mPort(iPort),
        mIAMode(iIAMode)
    {
        if(iIAMode)
            mProgram = new IAProgram();
        else
            mProgram = new HumanProgram();
    }

    ClientApplication::~ClientApplication()
    {
        delete mProgram;
        mClient->stop();
    }

    void ClientApplication::Start()
    {
        assert(mClient == nullptr);
        mClient = std::make_unique<httplib::Client>(mHost, mPort);

        //We connect the client to the server and send in the request the informations of the client

        nlohmann::json clientData;
        clientData["name"] = mName;

        auto res = mClient->Post("/client_connect", clientData.dump(), "application/json");

        if(res)
        {
            if(res->status == 200)
            {
                auto answer = nlohmann::json::parse(res->body);
                //TODO handle invalid json

                if(answer["connection_status"] == "success") {
                    mIsRunning = true;
                    std::cout<<"Connection to the server successfull !"<<std::endl;
                }
                else
                    std::cout<<"Error when connecting to the server : "<<answer["message"]<<std::endl;
            }
            else
                std::cout<<"Invalid status code when connecting: "<<res->status<<std::endl;
        }
        else
            std::cout<<"Unable to connect to the server"<<std::endl;
    }

    void ClientApplication::Run()
    {

        std::string number = mProgram->GetInput();

        nlohmann::json jsonAnswer;
        jsonAnswer["name"] = mName;
        jsonAnswer["number"] = number;

        auto res = mClient->Post("/client_answer", jsonAnswer.dump(), "application/json");

        if(res)
        {
            if(res->status == 200) {

                //TODO handle invalid json
                auto serverAnswer = nlohmann::json::parse(res->body);
                if(serverAnswer["server_answer_type"] == "invalid_number")
                    std::cout<<"Invalid number entered !"<<std::endl;
                else if(serverAnswer["server_answer_type"] == "number_check") {
                    auto answer = serverAnswer["server_answer"];
                    if(answer == "lower")
                    {
                        std::cout<<"It's lower !"<<std::endl;
                        mProgram->GetLowerValue();
                    }
                    else if(answer == "upper")
                    {
                        std::cout<<"It's upper !"<<std::endl;
                        mProgram->GetUpperValue();
                    }
                    else
                    {
                        std::cout<<"Correct answer ! Your score: "<<serverAnswer["score"]<<std::endl;
                        mIsRunning = false;
                        return;
                    }
                }
                else if(serverAnswer["server_answer_type"] == "limit_exceeded") {
                    std::cout<<"You exceeded the number of tries. The good answer was:"<<serverAnswer["good_answer"]<<std::endl;
                    mIsRunning = false;
                    return;
                }
            }
            else
                std::cout<<"Error : bad answer status : "<<res->status<<std::endl;
        }
        else
            std::cout<<"Error while sending the answer"<<std::endl;
    }

}