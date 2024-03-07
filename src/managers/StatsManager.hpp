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

    static ghc::filesystem::path getLevelSaveFilePath(GJGameLevel* level = m_level);

public:
    StatsManager() = delete;

    // main functions
    static LevelStats loadLevelStats(GJGameLevel* level);
    static void logDeath(float percent);
    static void logRun(Run run);

    // utility functions
    static std::string getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunKey(std::string runKey);
    static Session* getSession();
    static void setSessionLastPlayed(long long lastPlayed);
    static void scheduleCreateNewSession(bool scheduled);
    static bool hasPlayedLevel();
    static std::string toPercentStr(int percent, int precision = 2);
    static std::string toPercentStr(float percent, int precision = 2);
};