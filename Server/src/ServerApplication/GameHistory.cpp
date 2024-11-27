#include "ServerApplication/GameHistory.hpp"

namespace nApplication
{

    nlohmann::json GameHistory::ToJson() const
    {
        nlohmann::json  json;
        json["player_name"] = mPlayerName;
        json["player_ip"] = mPlayerIP;
        json["begin_time"] = std::chrono::duration_cast<std::chrono::milliseconds>(mBeginTime.time_since_epoch()).count();
        json["end_time"] = std::chrono::duration_cast<std::chrono::milliseconds>(mEndTime.time_since_epoch()).count();
        json["score"] = mScore;
        json["game_state"] = mGameState;
        return json;
    }

    void GameHistory::FromJson(const nlohmann::json& iJson)
    {
        mPlayerName = iJson["player_name"];
        mPlayerIP = iJson["player_ip"];
        mScore = iJson["score"];
        mGameState = iJson["game_state"];
        mBeginTime = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds (iJson["begin_time"].get<long long>()));
        mEndTime = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds (iJson["end_time"].get<long long>()));
    }

} // nApplication