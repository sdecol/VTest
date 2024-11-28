#include "ClientApplication/ClientApplication.hpp"

#include "ClientApplication/ClientInput/HumanInput.hpp"
#include "ClientApplication/ClientInput/IAInput.hpp"

#include "httplib/httplib.h"
#include "nlohmann/json.hpp"

using namespace std::chrono_literals;

namespace nApplication
{

    ClientApplication::ClientApplication(const std::string& iClientName, const std::string& iHost, int iPort,
                                         bool iIAMode) :
            mName(iClientName),
            mHost(iHost),
            mPort(iPort),
            mIAMode(iIAMode)
    {
        if (iIAMode)
            mPlayerInput = new IAInput();
        else
            mPlayerInput = new HumanInput();
    }

    ClientApplication::~ClientApplication()
    {
        delete mPlayerInput;
    }

    void ClientApplication::Start()
    {
        assert(mClient == nullptr);
        mClient = std::make_unique<httplib::Client>(mHost, mPort);

        //We connect the client to the server and send in the request the informations of the client

        nlohmann::json clientData;
        clientData["id"] = mClientID;
        clientData["is_ia"] = mIAMode;
        clientData["name"] = mName;

        //Connecting to the server
        auto res = mClient->Post("/client_connect", clientData.dump(), "application/json");

        if (res)
        {
            if (res->status == 200)
            {
                nlohmann::json answer;

                //We check if json data received from the request is valid
                try
                {
                    answer = nlohmann::json::parse(res->body);
                }
                catch(const std::exception& e)
                {
                    std::cerr<<"Can't read the request body"<<std::endl;
                    return;
                }

                // Client connected successfully, we retrieve its id from the server
                if (answer["connection_status"] == "success")
                {
                    mIsRunning = true;
                    mClientID = answer["client_id"];
                    std::cout << "Connection to the server successfull !" << std::endl;
                    //Starting sending ping messages
                    mPingThread = std::thread([this]()
                                              {
                                                  SendPing();
                                              });
                }
                else
                    std::cout << "Error when connecting to the server : " << answer["message"] << std::endl;
            }
            else if(res->status == 400) // The server returned an error when trying to read body as json
                std::cerr<<"The server couldn't read the last request"<<std::endl;
            else
                std::cout << "Invalid status code when connecting: " << res->status << std::endl;
        }
        else
            std::cout << "Unable to connect to the server" << std::endl;
    }

    void ClientApplication::Run()
    {
        //Getting the number to send to the server

        std::string number = mPlayerInput->GetInput();

        if(!mIsRunning) // in cas we can't reach the server from the ping request
            return;

        nlohmann::json jsonAnswer;
        jsonAnswer["id"] = mClientID;
        jsonAnswer["name"] = mName;
        jsonAnswer["number"] = number;

        //Sending the number to the server
        auto res = mClient->Post("/client_answer", jsonAnswer.dump(), "application/json");

        if (res)
        {
            if (res->status == 200)
            {

                nlohmann::json  serverAnswer;

                //Reading the answer of the server from the request body
                try
                {
                    serverAnswer = nlohmann::json::parse(res->body);
                }
                catch(std::exception& e)
                {
                    std::cerr<<"Can't read the request body"<<std::endl;
                    return;
                }

                //We check the answer of the server

                if (serverAnswer["server_answer_type"] == "invalid_number")
                {
                    std::cout << "Invalid number entered !" << std::endl;
                    return; //Nothing to do, we just run the next cycle
                }

                //The server processed the number entered, we check what kind of answer it returned
                if (serverAnswer["server_answer_type"] == "number_check")
                {
                    ProcessServerAnswer(serverAnswer);
                }
                else if (serverAnswer["server_answer_type"] == "limit_exceeded")
                {
                    std::cout << "You exceeded the number of tries. The good answer was:" << serverAnswer["good_answer"]
                              << std::endl;
                    DisplayHistory(serverAnswer);
                    mIsRunning = false;
                }
            }
            else if(res->status == 400) // Server failed to read body as json
                std::cerr<<"The server couldn't read the last request"<<std::endl;
            else
                std::cout << "Error : bad answer status : " << res->status << std::endl;
        }
        else
            std::cout << "Error while sending the answer" << std::endl;
    }

    void ClientApplication::Quit()
    {
        if(mPingThread.joinable())
            mPingThread.join();
        mClient->stop();
    }


    void ClientApplication::ProcessServerAnswer(const nlohmann::json& iServerAnswer)
    {
        //Checking the answer received from the server after the client sent his answer concerning the number to guess

        auto answer = iServerAnswer["server_answer"];
        if (answer == "lower")
        {
            std::cout << "It's lower !" << std::endl;
            mPlayerInput->GetLowerValue();
        }
        else if (answer == "higher")
        {
            std::cout << "It's higher !" << std::endl;
            mPlayerInput->GetHigherValue();
        }
        else
        {
            std::cout << "Correct answer ! Your score: " << iServerAnswer["score"] << std::endl;
            DisplayHistory(iServerAnswer);
            mIsRunning = false;
        }
    }

    void ClientApplication::SendPing()
    {
        while (mIsRunning)
        {
            //We send one ping per second only
             std::this_thread::sleep_for(1000ms);

            //We send a ping to notify the server that this client is still connected
            nlohmann::json pingBody;
            pingBody["name"] = mName;
            pingBody["id"] = mClientID;

            auto pingRes = mClient->Post("/ping", pingBody.dump(), "application/json");

            if (!pingRes || pingRes->status != 200)
            {
                std::cout << "Can't reach the server. Enter any value to exit" << std::endl;
                mIsRunning = false;
            }
        }
    }

    void ClientApplication::DisplayHistory(const nlohmann::json& iServerAnswer) const
    {
        if(mIAMode)
            return;

        //We check that the json data received from the server is valid
        nlohmann::json history;
        try
        {
            history = iServerAnswer["history"];
        }
        catch (const std::exception& e)
        {
            std::cerr << "Couldn't read history from the server !"<<std::endl;
        }

        //We display the best scores of this player
        std::cout<<std::endl;
        std::cout<<"============BEST SCORES============"<<std::endl;

        for(const auto& jsonObject : history)
        {
            std::cout<<" - "<<jsonObject["score"]<<std::endl;
        }

        std::cout<<"==================================="<<std::endl;
    }
}