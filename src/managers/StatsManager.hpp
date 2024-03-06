#pragma once

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;

typedef std::map<std::string, int> Deaths;
typedef std::map<std::string, int> Runs;
typedef std::map<std::string, bool> NewBests;

struct Run {
    float start;
    float end;
};

typedef struct {
    long long lastPlayed;
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    float currentBest;
} Session;

template <>
    struct matjson::Serialize<Session> {
        static Session from_json(const matjson::Value& value) {
            return Session {
                .lastPlayed = value["deaths"].as<long long>(),
                .deaths = value["deaths"].as<Deaths>(),
                .runs = value["deaths"].as<Runs>(),
                .newBests = value["deaths"].as<NewBests>(),
                .currentBest = static_cast<float>(value["deaths"].as_double()),
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

//this struct is the one that will be saved as json
typedef struct {
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    float currentBest;
    std::vector<Session> sessions;
} levelProgressSave;

template <>
    struct matjson::Serialize<levelProgressSave> {
        static levelProgressSave from_json(const matjson::Value& value) {
            return levelProgressSave {
                .deaths = value["deaths"].as<Deaths>(),
                .runs = value["runs"].as<Runs>(),
                .newBests = value["newBests"].as<NewBests>(),
                .currentBest = static_cast<float>(value["currentBest"].as_double()),
                .sessions = value["sessions"].as<std::vector<Session>>()
            };
        }
        static matjson::Value to_json(const levelProgressSave& value) {
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
    static std::map<std::string, bool> m_playedLevels;

    static Deaths m_deaths;
    static Runs m_runs;
    static NewBests m_newBests;
    static float m_currentBest;

    static std::vector<Session> m_sessions;
    static Session* m_currentSession;
    static bool m_scheduleCreateNewSession;
    static ghc::filesystem::path m_savesFolderPath;

    // internal functions
    static void createNewSession();
    static void saveData();

public:
    StatsManager() = delete;

    // main functions
    static void loadLevelStats(GJGameLevel* level);
    static void logDeath(float percent);
    static void logRun(Run run);

    // utility functions
    static std::string getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunKey(std::string runKey);
    static Session getSession();
    static void setSessionLastPlayed(long long lastPlayed);
    static void scheduleCreateNewSession(bool scheduled);
    static bool hasPlayedLevel();
    static ghc::filesystem::path getSavesDir();
};