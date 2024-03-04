#include "StatsManager.hpp"

using namespace geode::prelude;

//return a splitted version of the string provided, devided by the delim
std::vector<std::string> splitStr(std::string str, std::string delim) {
    size_t posStart = 0;
    size_t posEnd;
    size_t delimLen = delim.length();

    std::vector<std::string> res;

    while ((posEnd = str.find(delim, posStart)) != std::string::npos) {
        std::string token = str.substr(posStart, posEnd - posStart);
        posStart = posEnd + delimLen;
        res.push_back(token);
    }

    res.push_back(str.substr(posStart));
    return res;
}

/* static member variables
=========================== */
GJGameLevel* StatsManager::m_level = nullptr;
std::map<std::string, bool> StatsManager::m_playedLevels{};

Deaths StatsManager::m_totalDeaths{};
Runs StatsManager::m_totalRuns{};
NewBests StatsManager::m_newBests{};
float StatsManager::m_currentBest = 0;

std::vector<Session> StatsManager::m_sessions{};
Session* StatsManager::m_currentSession = nullptr;

/* main functions
================== */
void StatsManager::loadLevelStats(GJGameLevel* level) {
    if (!level) return;
    auto levelKey = StatsManager::getLevelKey(level);

    m_level = level;
    m_playedLevels[levelKey] = true;

    /* TODO: load level save data
     *
     * (0) load save data
     *   (a) m_newBests    (default: calculate from m_personalBests)
     *   (b) m_currentBest (default: level->m_normalPercent.value())
     *   (c) m_sessions
     *
     * (1) calculate derived data
     *   (a) m_totalDeaths: sum of all deaths across all sessions
     *   (b) m_totalRuns:   sum of all runs across all sessions
     *
    */

   // set m_currentSession to the most recent session
   // m_currentSession = &sessions[sessions.size() - 1];
}

void StatsManager::logDeath(float percent) {
    m_currentSession->deaths[percent]++;

    if (percent > m_currentBest) {
        m_currentBest = percent;
        m_newBests[percent] = true;
    }

    if (percent > m_currentSession->currentBest) {
        m_currentSession->currentBest = percent;
        m_currentSession->newBests[percent] = true;
    }
}

void StatsManager::logRun(Run run) {
    auto runKey = fmt::format("{}-{}", run.start, run.end);

    m_currentSession->runs[runKey]++;
}

/* utility functions
===================== */
std::string StatsManager::getLevelKey(GJGameLevel* level) {
    if (!level) return "-1";

    auto levelType = static_cast<int>(level->m_levelType);
    if (!levelType) levelType = 3;

    return fmt::format("{}-{}-{}-{}",
        level->m_levelID.value(),
        levelType,
        level->m_dailyID.value(),
        int(level->m_gauntletLevel)
    );
}

Run StatsManager::splitRunStr(std::string run) {
    auto runSplit = splitStr(run, "-");

    auto start = std::stof(runSplit[0]);
    auto end = std::stof(runSplit[1]);

    return Run(start, end);
}

Session StatsManager::getSession() {
    return *m_currentSession;
}