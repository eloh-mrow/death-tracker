#pragma once

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;

typedef std::map<std::string, int> Deaths;
typedef std::map<std::string, int> Runs;
typedef std::set<std::string> NewBests;

typedef struct {
    float start;
    float end;
} Run;

typedef struct {
    long long lastPlayed;
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    float currentBest;
} Session;

typedef struct {
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    float currentBest;
    std::vector<Session> sessions;
} LevelStats;

// matjson fuckery
template <>
struct matjson::Serialize<Session> {
    static Session from_json(const matjson::Value& value) {
        return Session {
            .lastPlayed = value["lastPlayed"].as<long long>(),
            .deaths = value["deaths"].as<Deaths>(),
            .runs = value["runs"].as<Runs>(),
            .newBests = value["newBests"].as<NewBests>(),
            .currentBest = std::stof(value["currentBest"].as_string()),
        };
    }

    static matjson::Value to_json(const Session& value) {
        auto obj = matjson::Object();
        obj["lastPlayed"] = value.lastPlayed;
        obj["deaths"] = value.deaths;
        obj["runs"] = value.runs;
        obj["newBests"] = value.newBests;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value.currentBest;
        obj["currentBest"] = ss.str();
        return obj;
    }
};

template <>
struct matjson::Serialize<LevelStats> {
    static LevelStats from_json(const matjson::Value& value) {
        return LevelStats {
            .deaths = value["deaths"].as<Deaths>(),
            .runs = value["runs"].as<Runs>(),
            .newBests = value["newBests"].as<NewBests>(),
            .currentBest = static_cast<float>(value["currentBest"].as_double()),
            .sessions = value["sessions"].as<std::vector<Session>>()
        };
    }

    static matjson::Value to_json(const LevelStats& value) {
        auto obj = matjson::Object();
        obj["deaths"] = value.deaths;
        obj["runs"] = value.runs;
        obj["newBests"] = value.newBests;
        obj["currentBest"] = value.currentBest;
        obj["sessions"] = value.sessions;
        return obj;
    }
};

class StatsManager {
private:
    static GJGameLevel* m_level;
    static std::set<std::string> m_playedLevels;

    static Deaths m_deaths;
    static Runs m_runs;
    static NewBests m_newBests;
    static float m_currentBest;

    static std::vector<Session> m_sessions;
    static bool m_scheduleCreateNewSession;
    static ghc::filesystem::path m_savesFolderPath;

    // internal functions
    static void saveData();
    static LevelStats loadData(GJGameLevel* level);
    static std::tuple<NewBests, float> calcNewBests(GJGameLevel* level);

public:
    StatsManager() = delete;

    // main functions
    static LevelStats loadLevelStats(GJGameLevel* level);
    static void logDeath(float percent);
    static void logRun(Run run);

    // utility functions
    static long long getNowSeconds();
    static std::string getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunKey(std::string runKey);
    static Session* getSession();
    static void updateSessionLastPlayed(bool save = false);
    static void scheduleCreateNewSession(bool scheduled);
    static bool hasPlayedLevel();
    static std::string toPercentStr(int percent, int precision = 2);
    static std::string toPercentStr(float percent, int precision = 2);
    static ghc::filesystem::path getLevelSaveFilePath(GJGameLevel* level = m_level);
};