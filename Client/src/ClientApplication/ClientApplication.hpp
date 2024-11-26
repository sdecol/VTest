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

        // Getters / setters

        [[nodiscard]]
        inline bool IsRunning() const noexcept{ return mIsRunning; }

    public:

        //Connects to the server
        void Start();

        void Run();

    private:

        std::string mName;
        std::string mHost;
        int mPort;
        bool mIsRunning = false;

        std::unique_ptr<httplib::Client> mClient = nullptr;
    };

} //nApplication