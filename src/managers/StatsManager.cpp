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

Deaths StatsManager::m_deaths{};
Runs StatsManager::m_runs{};
NewBests StatsManager::m_newBests{};
float StatsManager::m_currentBest = 0;

std::vector<Session> StatsManager::m_sessions{};
Session* StatsManager::m_currentSession = nullptr;
bool StatsManager::m_scheduleCreateNewSession = false;

/* main functions
================== */
void StatsManager::loadLevelStats(GJGameLevel* level) {
    if (!level) return;
    auto levelKey = StatsManager::getLevelKey(level);

    m_level = level;

    log::info("StatsManager::loadLevelStats()");

    /* TODO: load level save data
     *
     * - m_deaths
     * - m_runs
     * - m_newBests    (default: calculate from m_personalBests)
     * - m_currentBest (default: level->m_normalPercent.value())
     * - m_sessions
     *
    */

   // update m_currentSession, can be nullptr (no session)
   if (m_sessions.size()) m_currentSession = &m_sessions[m_sessions.size() - 1];
}

void StatsManager::logDeath(float percent) {
    // try to create a new session
    StatsManager::createNewSession();
    if (!m_currentSession) return;
    log::info("StatsManager::logDeath() -- {:.2f}", percent);

    m_deaths[percent]++;
    m_currentSession->deaths[percent]++;

    if (percent > m_currentBest) {
        m_currentBest = percent;
        m_newBests[percent] = true;
    }

    if (percent > m_currentSession->currentBest) {
        m_currentSession->currentBest = percent;
        m_currentSession->newBests[percent] = true;
    }

    StatsManager::saveData();
}

void StatsManager::logRun(Run run) {
    // try to create a new session
    StatsManager::createNewSession();
    if (!m_currentSession) return;
    log::info("StatsManager::logRun() -- {:.2f}-{:.2f}", run.start, run.end);

    auto runKey = fmt::format("{}-{}", run.start, run.end);
    m_runs[runKey]++;
    m_currentSession->runs[runKey]++;

    StatsManager::saveData();
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

Run StatsManager::splitRunKey(std::string runKey) {
    auto runKeySplit = splitStr(runKey, "-");

    auto start = std::stof(runKeySplit[0]);
    auto end = std::stof(runKeySplit[1]);

    return Run(start, end);
}

Session StatsManager::getSession() {
    return *m_currentSession;
}

void StatsManager::setSessionLastPlayed(long long lastPlayed) {
    if (!m_currentSession) return;
    m_currentSession->lastPlayed = lastPlayed;
}

void StatsManager::scheduleCreateNewSession(bool scheduled) {
    log::info("StatsManager::scheduleCreateNewSession() -- {}", scheduled);
    m_scheduleCreateNewSession = scheduled;
}

bool StatsManager::hasPlayedLevel() {
    if (!m_level) return false;
    auto levelKey = StatsManager::getLevelKey();
    return m_playedLevels[levelKey];
}

/* internal functions
======================= */
void StatsManager::saveData() {
    log::info("StatsManager::saveData()");
    /* TODO: save level data
     *
     * <save_dir>/levels/<levelKey>.json
     *
     * {
     *   "deaths": {
     *     "1.23": 8,
     *     "7.23": 12,
     *     "24.21": 2,
     *     ...
     *   },
     *
     *   "runs": {
     *     "2.34-7.89": 2,
     *     "23.11-67.32": 5,
     *     ...
     *   },
     *
     *   "newBests": {
     *     "1.23": true,
     *     "24.21": true,
     *     "32.45": true,
     *     ...
     *   },
     *
     *   "currentBest": 32.45,
     *
     *   "sessions": [{
     *     "lastPlayed": 123456789,
     *     "deaths": {...},
     *     "runs": {...},
     *     "newBests": {...},
     *     "currentBest": 12.67
     *   }, {...}, ...],
     * }
     *
    */
}

void StatsManager::createNewSession() {
    // new sessions can be scheduled
    // and are created when the player dies
    if (!m_scheduleCreateNewSession) return;
    log::info("StatsManager::createNewSession()");

    // the user has played the level
    // if a new session is created
    auto levelKey = StatsManager::getLevelKey();
    m_playedLevels[levelKey] = true;

    // create the new session
    auto session = Session(-1, {}, {}, {}, -1);

    m_sessions.push_back(session);
    m_currentSession = &session;
}