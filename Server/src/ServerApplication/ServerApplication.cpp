#include "ServerApplication/ServerApplication.hpp"

#include <stdexcept>

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
        mServer.listen("0.0.0.0", mPort);

        mServer.Get("/", [](const httplib::Request& iReq, httplib::Response& oRes) {
            oRes.set_content("Hello, World! Your request was received.", "text/plain");
        });
    }

    void ServerApplication::Quit()
    {
        mServer.stop();
    }
} // nApplication
