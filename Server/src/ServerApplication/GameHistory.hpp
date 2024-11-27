#pragma once

#include "nlohmann/json.hpp"

#include <chrono>
#include <string>

namespace nApplication
{
    class GameHistory
    {
    public:

        enum GameState
        {
            Win = 0,
            Loose = 1,
            GaveUp = 2,
            Pending = 3
        };

    public:

        GameHistory() = default;

        ~GameHistory() = default;

    public:

        inline void RecordEndTime()
        {
            mEndTime = std::chrono::system_clock::now();
        }

        //Converts the data to json format
        [[nodiscard]]
        nlohmann::json ToJson() const;

        //Fill data with the given json object
        void FromJson(const nlohmann::json& iJson);

    public:

        std::string mPlayerName;
        std::chrono::time_point<std::chrono::system_clock> mBeginTime;
        std::chrono::time_point<std::chrono::system_clock> mEndTime;
        int mNbTry = 0;
        GameState mGameState = GameState::Pending;
    };

} // nApplication
