#pragma once

#include "httplib/httplib.h"

#include <string>

namespace nApplication
{

    class ClientApplication
    {
    public:

        ClientApplication( const std::string& iClientName, const std::string& iHost,int iPort);
        ~ClientApplication();

    public:

        //Connects to the server
        void Start();

    private:

        std::string mName;
        std::string mHost;
        int mPort;

        httplib::Client* mClient = nullptr;
    };

} //nApplication