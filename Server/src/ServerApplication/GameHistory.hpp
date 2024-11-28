#pragma once

#include "nlohmann/json.hpp"

#include <chrono>
#include <string>

namespace nApplication
{
    /**
     *  A class used to store a game history for each game played for any player (if not IA)
     */
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

        //Construction / Destruction

        GameHistory() = default;
        ~GameHistory() = default;

    public:

        //Records the begin time for this game
        inline void RecordBeginTime()
        {
            mBeginTime = std::chrono::system_clock::now();
        }

        //Records the end time for this game
        inline void RecordEndTime() noexcept
        {
            mEndTime = std::chrono::system_clock::now();
        }

        //Converts the data to json format
        [[nodiscard]]
        nlohmann::json ToJson() const;

        //Fills data with the given json object
        void FromJson(const nlohmann::json& iJson);

    public:

        std::string mPlayerName; // The name of the player
        std::string mPlayerIP; // The ip adress of the client
        std::chrono::time_point<std::chrono::system_clock> mBeginTime; // Start time of the represented game
        std::chrono::time_point<std::chrono::system_clock> mEndTime; // End time of the represented game
        int mScore = 0; // Score recorded for this game
        GameState mGameState = GameState::Pending; // The last recorded state of the game
    };

} // nApplication
