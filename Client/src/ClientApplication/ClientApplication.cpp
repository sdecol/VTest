#include "ClientApplication/ClientApplication.hpp"

namespace nApplication
{

    ClientApplication::ClientApplication(const std::string &iClientName, const std::string &iHost, int iPort) :
        mName(iClientName),
        mHost(iHost),
        mPort(iPort)
    {

    }

    ClientApplication::~ClientApplication()
    {
        if(mClient != nullptr)
        {
            mClient->stop();
            delete mClient;
        }
    }

    void ClientApplication::Start()
    {
        assert(mClient == nullptr);
        mClient = new httplib::Client("https:://" + mHost + ":"+std::to_string(mPort));
    }

}