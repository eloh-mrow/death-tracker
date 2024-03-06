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

// matjson fuckery
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

template <>
struct matjson::Serialize<LevelProgressSave> {
    static LevelProgressSave from_json(const matjson::Value& value) {
        return LevelProgressSave {
            .deaths = value["deaths"].as<Deaths>(),
            .runs = value["runs"].as<Runs>(),
            .newBests = value["newBests"].as<NewBests>(),
            .currentBest = static_cast<float>(value["currentBest"].as_double()),
            .sessions = value["sessions"].as<std::vector<Session>>()
        };
    }
    static matjson::Value to_json(const LevelProgressSave& value) {
        auto obj = matjson::Object();
        obj["deaths"] = value.deaths;
        obj["runs"] = value.runs;
        obj["newBests"] = value.newBests;
        obj["currentBest"] = value.currentBest;
        obj["sessions"] = value.sessions;
        return obj;
    }
};

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

ghc::filesystem::path StatsManager::m_savesFolderPath = Mod::get()->getSaveDir() / "levels";

/* main functions
================== */
LevelProgressSave StatsManager::getLevelStats(GJGameLevel* level) {
    if (!level) throw std::invalid_argument("GJGameLevel* level");
    log::info("StatsManager::getLevelStats()");

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath();

    if (ghc::filesystem::exists(levelSaveFilePath))
        return file::readJson(levelSaveFilePath).value().as<LevelProgressSave>();

    // get defaults for level stats
    // includes backwards compatibility for v1.x.x
    LevelProgressSave levelStats{};

    auto levelKey = StatsManager::getLevelKey(level);
    auto old__deaths = Mod::get()->getSavedValue<std::vector<int>>(levelKey);
    auto old__sessionDeaths = Mod::get()->getSavedValue<std::vector<int>>(levelKey + "-session");
    auto old__sessionBests = Mod::get()->getSavedValue<std::vector<bool>>(levelKey + "-session-bests");
    auto old__sessionTime = Mod::get()->getSavedValue<long long>(levelKey + "-session-time", -1);

    Deaths deaths{};
    Deaths sessionDeaths{};
    NewBests sessionBests{};
    float currentSessionBest = -1;

    for (int percent = 0; percent <= 100; percent++) {
        auto percentStr = StatsManager::toPercentStr(percent);

        deaths[percentStr] = old__deaths[percent];
        sessionDeaths[percentStr] = old__sessionDeaths[percent];
        sessionBests[percentStr] = old__sessionBests[percent];

        if (old__sessionBests[percent] && percent > currentSessionBest)
            currentSessionBest = static_cast<float>(percent);
    }

    levelStats.deaths = deaths;
    levelStats.runs = Runs();

    levelStats.sessions.push_back(Session(
        old__sessionTime,
        sessionDeaths,
        {}, // v1.x.x doesnt track runs
        sessionBests,
        currentSessionBest
    ));

    // calculate m_newBests, m_currentBest
    auto [newBests, currentBest] = StatsManager::calcNewBests();
    levelStats.newBests = newBests;
    levelStats.currentBest = currentBest;
    return levelStats;
}

void StatsManager::loadLevelStats(GJGameLevel* level) {
    if (!level) return;
    log::info("StatsManager::loadLevelStats()");

    auto levelStats = StatsManager::getLevelStats(level);

    m_level = level;
    m_deaths = levelStats.deaths;
    m_runs = levelStats.runs;
    m_newBests = levelStats.newBests;
    m_currentBest = levelStats.currentBest;
    m_sessions = levelStats.sessions;

   // update m_currentSession, can be nullptr (no session)
   if (m_sessions.size()) m_currentSession = &m_sessions[m_sessions.size() - 1];
}

void StatsManager::logDeath(float percent) {
    // try to create a new session
    StatsManager::createNewSession();
    if (!m_currentSession) return;
    log::info("StatsManager::logDeath() -- {:.2f}", percent);

    auto percentStr = StatsManager::toPercentStr(percent);

    m_deaths[percentStr]++;
    m_currentSession->deaths[percentStr]++;

    if (percent > m_currentBest) {
        m_currentBest = percent;
        m_newBests[percentStr] = true;
    }

    if (percent > m_currentSession->currentBest) {
        m_currentSession->currentBest = percent;
        m_currentSession->newBests[percentStr] = true;
    }

    StatsManager::saveData();
}

void StatsManager::logRun(Run run) {
    // try to create a new session
    StatsManager::createNewSession();
    if (!m_currentSession) return;
    log::info("StatsManager::logRun() -- {:.2f}-{:.2f}", run.start, run.end);

    auto runKey = fmt::format("{}-{}",
        StatsManager::toPercentStr(run.start),
        StatsManager::toPercentStr(run.end)
    );

    m_runs[runKey]++;
    m_currentSession->runs[runKey]++;

    StatsManager::saveData();
}

/* utility functions
===================== */
std::string StatsManager::getLevelKey(GJGameLevel* level) {
	if (!level) return "-1";

	auto levelId = std::to_string(level->m_levelID.value());

	// local level postfix
	if (level->m_levelType != GJLevelType::Saved)
		levelId += "-local";

	// daily/weekly postfix
	if (level->m_dailyID > 0)
		levelId += "-daily";

	// gauntlet level postfix
	if (level->m_gauntletLevel)
		levelId += "-gauntlet";

	return levelId;

    // if (!level) return "-1";

    // auto levelType = static_cast<int>(level->m_levelType);
    // if (!levelType) levelType = 3;

    // return fmt::format("{}-{}-{}-{}",
    //     level->m_levelID.value(),
    //     levelType,
    //     level->m_dailyID.value(),
    //     int(level->m_gauntletLevel)
    // );
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

    std::string levelKey = StatsManager::getLevelKey();
    if (levelKey == "-1") return;

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath();

    // create the json file if it doesnt exist
    if (!ghc::filesystem::exists(levelSaveFilePath)) {
        std::ofstream levelSaveFile(levelSaveFilePath);
        levelSaveFile.close();
    }

    // save the data
    LevelProgressSave levelSaveData{};
    levelSaveData.deaths = m_deaths;
    levelSaveData.runs = m_runs;
    levelSaveData.newBests = m_newBests;
    levelSaveData.currentBest = m_currentBest;
    levelSaveData.sessions = m_sessions;

    file::writeToJson(levelSaveFilePath, levelSaveData);

    /* <save_dir>/levels/<levelKey>.json
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
    if (levelKey == "-1") return;

    m_playedLevels[levelKey] = true;

    // create the new session
    auto session = Session(-1, {}, {}, {}, -1);

    m_sessions.push_back(session);
    m_currentSession = &session;
}

std::tuple<NewBests, float> StatsManager::calcNewBests() {
    NewBests newBests{};
    std::stringstream bestsStream(m_level->m_personalBests);
    std::string currentBest;
    int currentPercent = 0;

    while (getline(bestsStream, currentBest, ',')) {
        currentPercent += std::stoi(currentBest);
        auto percentStr = StatsManager::toPercentStr(currentPercent);
        newBests[percentStr] = true;
    }

    return {newBests, static_cast<float>(currentPercent)};
}

std::string StatsManager::toPercentStr(int percent) {
    return StatsManager::toPercentStr(static_cast<float>(percent));
}

std::string StatsManager::toPercentStr(float percent) {
    return fmt::format("{:.2f}", percent);
}

ghc::filesystem::path StatsManager::getLevelSaveFilePath() {
    ghc::filesystem::path filePath{};
    auto levelKey = StatsManager::getLevelKey();
    if (levelKey == "-1") return filePath;

    filePath = m_savesFolderPath / (levelKey + ".json");
    return filePath;
}
