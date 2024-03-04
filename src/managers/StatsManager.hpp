#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

typedef std::map<float, int> Deaths;
typedef std::map<std::string, int> Runs;
typedef std::map<float, bool> NewBests;

struct Run {
    float start;
    float end;
};

struct Session {
    long long lastPlayed;
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    float currentBest;
};

class StatsManager {
private:
    static GJGameLevel* m_level;
    static std::map<std::string, bool> m_playedLevels;

    static Deaths m_totalDeaths;
    static Runs m_totalRuns;
    static NewBests m_newBests;
    static float m_currentBest;

    static std::vector<Session> m_sessions;
    static Session* m_currentSession;

public:
    StatsManager() = delete;

    /* main functions */
    static void loadLevelStats(GJGameLevel* level);
    static void logDeath(float percent);
    static void logRun(Run run);

    /* utility functions */
    static std::string getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunStr(std::string run);
    static Session getSession();
};