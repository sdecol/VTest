#pragma once

#include "httplib/httplib.h"

#include <string>

namespace nApplication
{
    class ServerApplication
    {
    public:

        // Construction / Destruction
        ServerApplication( int iPort, int iLimit, int iMinValue, int iMaxValue);
        ~ServerApplication() = default;

        inline bool IsRunning() const noexcept { return mIsRunning; }

        // Initializes the server
        void Start();

        //Stops the server
        void Quit();

    private:

        httplib::Server mServer;

        bool mIsRunning = false;

        int mPort = 0;
        int mLimit = -1;
        int mMinValue = 0;
        int mMaxValue = 0;
    };

} //nApplication
