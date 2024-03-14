#include "StatsManager.hpp"
#include "../utils/Settings.hpp"
#include "../utils/Dev.hpp"

using namespace geode::prelude;

// return a splitted version of the string provided, devided by the delim
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
std::set<std::string> StatsManager::m_playedLevels{};

Deaths StatsManager::m_deaths{};
Runs StatsManager::m_runs{};
NewBests StatsManager::m_newBests{};
float StatsManager::m_currentBest = 0;

std::vector<Session> StatsManager::m_sessions{};
bool StatsManager::m_scheduleCreateNewSession = false;

ghc::filesystem::path StatsManager::m_savesFolderPath = Mod::get()->getSaveDir() / "levels";

/* main functions
================== */
LevelStats StatsManager::loadLevelStats(GJGameLevel* level) {
    auto levelStats = StatsManager::loadData(level);

    log::info("StatsManager::loadLevelStats() --\ndeaths.size() = {}\nruns.size() = {}\nnewBests.size() = {}\ncurrentBest = {}\nsessions.size() = {}",
        levelStats.deaths.size(),
        levelStats.runs.size(),
        levelStats.newBests.size(),
        levelStats.currentBest,
        levelStats.sessions.size()
    );

    m_level = level;
    m_deaths = levelStats.deaths;
    m_runs = levelStats.runs;
    m_newBests = levelStats.newBests;
    m_currentBest = levelStats.currentBest;
    m_sessions = levelStats.sessions;

    return levelStats;
}

void StatsManager::logDeath(float percent) {
    auto session = StatsManager::getSession();
    if (!session) return;
    log::info("StatsManager::logDeath() -- {:.2f}", percent);

    auto percentStr = StatsManager::toPercentStr(percent);

    m_deaths[percentStr]++;
    session->deaths[percentStr]++;

    if (percent > m_currentBest) {
        m_currentBest = percent;
        m_newBests.insert(percentStr);
    }

    if (percent > session->currentBest) {
        session->currentBest = percent;
        session->newBests.insert(percentStr);
    }

    StatsManager::updateSessionLastPlayed();
    StatsManager::saveData();
}

void StatsManager::logRun(Run run) {
    auto session = StatsManager::getSession();
    if (!session) return;
    log::info("StatsManager::logRun() -- {:.2f}-{:.2f}", run.start, run.end);

    auto runKey = fmt::format("{}-{}",
        StatsManager::toPercentStr(run.start),
        StatsManager::toPercentStr(run.end)
    );

    m_runs[runKey]++;
    session->runs[runKey]++;

    StatsManager::updateSessionLastPlayed();
    StatsManager::saveData();
}

/* utility functions
===================== */
long long StatsManager::getNowSeconds() {
    using namespace std::chrono;
    auto now = system_clock::now();
    return time_point_cast<seconds>(now).time_since_epoch().count();
}

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

Session* StatsManager::getSession() {
    log::info("StatsManager::getSession()");

    auto currentSession = &m_sessions[m_sessions.size() - 1];

    // new sessions can be scheduled
    // and are created when the player dies
    if (!m_scheduleCreateNewSession) return currentSession;
    m_scheduleCreateNewSession = false;

    auto levelKey = StatsManager::getLevelKey();
    if (levelKey == "-1") return currentSession;

    // the user has played the level
    // if a new session is created
    m_playedLevels.insert(levelKey);

    // create the new session
    auto session = Session {
        .lastPlayed = -1,
        .deaths = {},
        .runs = {},
        .newBests = {},
        .currentBest = -1
    };

    m_sessions.push_back(session);
    return &m_sessions[m_sessions.size() - 1];
}

void StatsManager::updateSessionLastPlayed(bool save) {
    auto now = StatsManager::getNowSeconds();
    auto session = StatsManager::getSession();

    session->lastPlayed = now;

    log::info("StatsManager::updateSessionLastPlayed()\nnow = {}\nsave = {}\n# of deaths = {}",
        now,
        save,
        session->deaths.size()
    );

    if (save && session->deaths.size() > 0)
        StatsManager::saveData();
}

void StatsManager::scheduleCreateNewSession(bool scheduled) {
    log::info("StatsManager::scheduleCreateNewSession() -- {}", scheduled);
    m_scheduleCreateNewSession = scheduled;
}

bool StatsManager::hasPlayedLevel() {
    if (!m_level) return false;
    auto levelKey = StatsManager::getLevelKey();
    return m_playedLevels.count(levelKey);
}

std::string StatsManager::toPercentStr(int percent, int precision) {
    return StatsManager::toPercentStr(static_cast<float>(percent));
}

std::string StatsManager::toPercentStr(float percent, int precision, bool fixRounding) {
    std::stringstream ss;
    float fixedPrecent = percent;
    if (percent > 0.5f && fixRounding && Mod::get()->getSettingValue<int64_t>("precision") == 0) fixedPrecent -= 0.5f;
    if (percent > 0.05f && fixRounding && Mod::get()->getSettingValue<int64_t>("precision") == 1) fixedPrecent -= 0.05f;
    ss << std::fixed << std::setprecision(precision) << fixedPrecent;
    return ss.str();
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
    LevelStats levelSaveData{};
    levelSaveData.deaths = m_deaths;
    levelSaveData.runs = m_runs;
    levelSaveData.newBests = m_newBests;
    levelSaveData.currentBest = m_currentBest;
    levelSaveData.sessions = m_sessions;

    auto indentation = Dev::MINIFY_SAVE_FILE
        ? matjson::NO_INDENTATION
        : 4;

    auto jsonStr = matjson::Value(levelSaveData).dump(indentation);
    auto _ = file::writeString(levelSaveFilePath, jsonStr);

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
     *   "newBests": ["1.23", "24.21", "32.45", ...],
     *   "currentBest": 32.45,
     *
     *   "sessions": [{
     *     "lastPlayed": 123456789,
     *     "deaths": {...},
     *     "runs": {...},
     *     "newBests": [...],
     *     "currentBest": 12.67
     *   }, {...}, ...],
     * }
     *
    */
}

LevelStats StatsManager::loadData(GJGameLevel* level) {
    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath(level);
    log::info("StatsManager::loadData() --\n{}", levelSaveFilePath);

    if (ghc::filesystem::exists(levelSaveFilePath))
        return file::readJson(levelSaveFilePath).value().as<LevelStats>();

    // get defaults for level stats
    // includes backwards compatibility for v1.x.x
    LevelStats levelStats{};

    auto levelKey = StatsManager::getLevelKey(level);
    auto old__deaths = Mod::get()->getSavedValue<std::vector<int>>(levelKey);
    auto old__sessionDeaths = Mod::get()->getSavedValue<std::vector<int>>(levelKey + "-session");
    auto old__sessionBests = Mod::get()->getSavedValue<std::vector<bool>>(levelKey + "-session-bests");
    auto old__sessionTime = Mod::get()->getSavedValue<long long>(levelKey + "-session-time", -1);
    auto old__platBests = Mod::get()->getSavedValue<std::vector<bool>>(levelKey + "-plat-bests");

    Deaths deaths{};
    Deaths sessionDeaths{};
    NewBests sessionBests{};
    float currentSessionBest = -1;

    for (int percent = 0; percent < old__deaths.size(); percent++) {
        auto percentStr = StatsManager::toPercentStr(percent);

        if (old__deaths[percent] > 0)
            deaths[percentStr] = old__deaths[percent];
    }

    for (int percent = 0; percent < old__sessionDeaths.size(); percent++) {
        auto percentStr = StatsManager::toPercentStr(percent);

        if (old__sessionDeaths[percent] > 0)
            sessionDeaths[percentStr] = old__sessionDeaths[percent];

        // some older versions of v1.x.x do not
        // have session best tracking
        if (!old__sessionBests.size()) continue;

        if (old__sessionBests[percent]) {
            sessionBests.insert(percentStr);

            if (percent > currentSessionBest)
                currentSessionBest = static_cast<float>(percent);
        }
    }

    levelStats.deaths = deaths;
    levelStats.runs = Runs();

    levelStats.sessions.push_back(Session {
        .lastPlayed = !sessionDeaths.size() ? -2 : old__sessionTime,
        .deaths = sessionDeaths,
        .runs = {}, // v1.x.x doesnt track runs
        .newBests = sessionBests,
        .currentBest = currentSessionBest
    });

    // calculate m_newBests, m_currentBest
    NewBests newBests{};
    float currentBest = 0;

    if (level->isPlatformer()) {
        for (int checkpt = 0; checkpt < old__platBests.size(); checkpt++) {
            auto checkptStr = StatsManager::toPercentStr(checkpt);

            if (old__platBests[checkpt]) {
                newBests.insert(checkptStr);

                if (checkpt > currentBest)
                    currentBest = static_cast<float>(checkpt);
            }
        }
    } else {
        const auto [_newBests, _currentBest] = StatsManager::calcNewBests(level);
        newBests = _newBests;
        currentBest = _currentBest;
    }

    levelStats.newBests = newBests;
    levelStats.currentBest = currentBest;

    // default deaths to newBests x1
    if (!deaths.size()) {
        NewBests::iterator itr;
        auto start = levelStats.newBests.begin();
        auto end = levelStats.newBests.end();

        for (itr = start; itr != end; itr++) {
            auto percentStr = *itr;
            if (percentStr == "100.00") continue;

            levelStats.deaths[percentStr] = 1;
        }
    }

    return levelStats;
}

std::tuple<NewBests, float> StatsManager::calcNewBests(GJGameLevel* level) {
    NewBests newBests{};
    std::stringstream bestsStream(level->m_personalBests);
    std::string currentBest;
    int currentPercent = 0;

    while (getline(bestsStream, currentBest, ',')) {
        currentPercent += std::stoi(currentBest);
        auto percentStr = StatsManager::toPercentStr(currentPercent);
        newBests.insert(percentStr);
    }

    return std::tuple<NewBests, float>(newBests, static_cast<float>(currentPercent));
}

ghc::filesystem::path StatsManager::getLevelSaveFilePath(GJGameLevel* level) {
    ghc::filesystem::path filePath{};
    auto levelKey = StatsManager::getLevelKey(level);
    if (levelKey == "-1") return filePath;

    filePath = m_savesFolderPath / (levelKey + ".json");
    return filePath;
}

bool StatsManager::ContainsAtIndex(int startIndex, std::string check, std::string str){
    if (startIndex + check.length() >= str.length()) return false;
    bool toReturn = true;

    for (int i = 0; i < check.length(); i++)
    {
        if (str[startIndex + i] != check[i]) toReturn = false;
    }
    
    return toReturn;
}