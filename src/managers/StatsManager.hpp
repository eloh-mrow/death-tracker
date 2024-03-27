#pragma once

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;

typedef std::map<std::string, int> Deaths;
typedef std::map<std::string, int> Runs;
typedef std::set<int> NewBests;

typedef struct {
    int start;
    int end;
} Run;

typedef struct {
    long long lastPlayed;
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    int currentBest;
} Session;

typedef struct {
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    int currentBest;
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
            .currentBest = value["currentBest"].as_int(),
        };
    }

    static matjson::Value to_json(const Session& value) {
        auto obj = matjson::Object();
        obj["lastPlayed"] = value.lastPlayed;
        obj["deaths"] = value.deaths;
        obj["runs"] = value.runs;
        obj["newBests"] = value.newBests;
        obj["currentBest"] = value.currentBest;
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
            .currentBest = value["currentBest"].as_int(),
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

    static LevelStats m_levelStats;

    static bool m_scheduleCreateNewSession;
    static ghc::filesystem::path m_savesFolderPath;

    // internal functions
    static void saveData();
    static LevelStats loadData(GJGameLevel* level);
    static std::tuple<NewBests, int> calcNewBests(GJGameLevel* level);

public:
    StatsManager() = delete;

    // main functions
    static void loadLevelStats(GJGameLevel* level);
    static LevelStats getLevelStats(GJGameLevel* level);
    static void logDeath(int percent);
    static void logRun(Run run);

    // utility functions
    static long long getNowSeconds();
    static std::string getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunKey(std::string runKey);
    static Session* getSession();
    static void updateSessionLastPlayed(bool save = false);
    static void scheduleCreateNewSession(bool scheduled);
    static bool hasPlayedLevel();
    static std::string toPercentKey(int percent);
    static ghc::filesystem::path getLevelSaveFilePath(GJGameLevel* level = m_level);
    static bool ContainsAtIndex(int startIndex, std::string check, std::string str);
};