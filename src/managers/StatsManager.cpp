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
bool StatsManager::m_scheduleCreateNewSession = false;

LevelStats StatsManager::m_levelStats{};

ghc::filesystem::path StatsManager::m_savesFolderPath = Mod::get()->getSaveDir() / "levels";

std::vector<std::string> StatsManager::m_AllFontsMap{
    "bigFont.fnt",
    "chatFont.fnt",
    "goldFont.fnt",
    "gjFont01.fnt",
    "gjFont02.fnt",
    "gjFont03.fnt",
    "gjFont04.fnt",
    "gjFont05.fnt",
    "gjFont06.fnt",
    "gjFont07.fnt",
    "gjFont08.fnt",
    "gjFont09.fnt",
    "gjFont10.fnt",
    "gjFont11.fnt",
    "gjFont12.fnt",
    "gjFont13.fnt",
    "gjFont14.fnt",
    "gjFont15.fnt",
    "gjFont16.fnt",
    "gjFont17.fnt",
    "gjFont18.fnt",
    "gjFont19.fnt",
    "gjFont20.fnt",
    "gjFont21.fnt",
    "gjFont22.fnt",
    "gjFont23.fnt",
    "gjFont24.fnt",
    "gjFont25.fnt",
    "gjFont26.fnt",
    "gjFont27.fnt",
    "gjFont28.fnt",
    "gjFont29.fnt",
    "gjFont30.fnt",
    "gjFont31.fnt",
    "gjFont32.fnt",
    "gjFont33.fnt",
    "gjFont34.fnt",
    "gjFont35.fnt",
    "gjFont36.fnt",
    "gjFont37.fnt",
    "gjFont38.fnt",
    "gjFont39.fnt",
    "gjFont40.fnt",
    "gjFont41.fnt",
    "gjFont42.fnt",
    "gjFont43.fnt",
    "gjFont44.fnt",
    "gjFont45.fnt",
    "gjFont46.fnt",
    "gjFont47.fnt",
    "gjFont48.fnt",
    "gjFont49.fnt",
    "gjFont50.fnt",
    "gjFont51.fnt",
    "gjFont52.fnt",
    "gjFont53.fnt",
    "gjFont54.fnt",
    "gjFont55.fnt",
    "gjFont56.fnt",
    "gjFont57.fnt",
    "gjFont58.fnt",
    "gjFont59.fnt",
};

/* main functions
================== */
void StatsManager::loadLevelStats(GJGameLevel* level) {
    if (m_level == level) return;

    auto levelStats = StatsManager::loadData(level);

    // log::info("StatsManager::loadLevelStats() --\ndeaths.size() = {}\nruns.size() = {}\nnewBests.size() = {}\ncurrentBest = {}\nsessions.size() = {}",
    //     levelStats.deaths.size(),
    //     levelStats.runs.size(),
    //     levelStats.newBests.size(),
    //     levelStats.currentBest,
    //     levelStats.sessions.size()
    // );

    m_levelStats = levelStats;
    m_level = level;
}

LevelStats StatsManager::getLevelStats(GJGameLevel* level) {
    StatsManager::loadLevelStats(level);
    return m_levelStats;
}

void StatsManager::logDeath(int percent) {
    auto session = StatsManager::getSession();
    if (!session) return;
    // log::info("StatsManager::logDeath() -- {}%", percent);

    auto percentKey = StatsManager::toPercentKey(percent);

    m_levelStats.deaths[percentKey]++;
    session->deaths[percentKey]++;

    if (percent > m_levelStats.currentBest) {
        m_levelStats.currentBest = percent;
        m_levelStats.newBests.insert(percent);
    }

    if (percent > session->currentBest) {
        session->currentBest = percent;
        session->newBests.insert(percent);
    }

    StatsManager::updateSessionLastPlayed();
    StatsManager::saveData();
}

void StatsManager::logRun(Run run) {
    bool TrackRun = false;
    for (int i = 0; i < m_levelStats.RunsToSave.size(); i++)
    {
        if (m_levelStats.RunsToSave[i] == run.start){
            TrackRun = true;
            break;
        }
    }

    if (!TrackRun) return;

    auto session = StatsManager::getSession();
    if (!session) return;
    // log::info("StatsManager::logRun() -- {}% - {}%", run.start, run.end);

    auto runKey = fmt::format("{}-{}",
        StatsManager::toPercentKey(run.start),
        StatsManager::toPercentKey(run.end)
    );

    m_levelStats.runs[runKey]++;
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
}

Run StatsManager::splitRunKey(std::string runKey) {
    auto runKeySplit = splitStr(runKey, "-");

    auto start = std::stof(runKeySplit[0]);
    auto end = std::stof(runKeySplit[1]);

    return Run(start, end);
}

Session* StatsManager::getSession() {
    // log::info("StatsManager::getSession()");

    auto currentSession = &m_levelStats.sessions[m_levelStats.sessions.size() - 1];

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

    m_levelStats.sessions.push_back(session);
    return &m_levelStats.sessions[m_levelStats.sessions.size() - 1];
}

void StatsManager::updateSessionLastPlayed(bool save) {
    auto now = StatsManager::getNowSeconds();
    auto session = StatsManager::getSession();

    session->lastPlayed = now;

    // log::info("StatsManager::updateSessionLastPlayed()\nnow = {}\nsave = {}\n# of deaths = {}",
    //     now,
    //     save,
    //     session->deaths.size()
    // );

    if (save && session->deaths.size() > 0)
        StatsManager::saveData();
}

void StatsManager::scheduleCreateNewSession(bool scheduled) {
    // log::info("StatsManager::scheduleCreateNewSession() -- {}", scheduled);
    m_scheduleCreateNewSession = scheduled;
}

bool StatsManager::hasPlayedLevel() {
    if (!m_level) return false;
    auto levelKey = StatsManager::getLevelKey();
    return m_playedLevels.contains(levelKey);
}

std::string StatsManager::toPercentKey(int percent) {
    return std::to_string(percent);
}

/* internal functions
======================= */
void StatsManager::saveData() {
    // log::info("StatsManager::saveData()");

    std::string levelKey = StatsManager::getLevelKey();
    if (levelKey == "-1") return;

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath();

    // create the json file if it doesnt exist
    if (!ghc::filesystem::exists(levelSaveFilePath)) {
        std::ofstream levelSaveFile(levelSaveFilePath);
        levelSaveFile.close();
    }

    // save the data
    auto indentation = Dev::MINIFY_SAVE_FILE
        ? matjson::NO_INDENTATION
        : 4;

    auto jsonStr = matjson::Value(m_levelStats).dump(indentation);
    auto _ = file::writeString(levelSaveFilePath, jsonStr);

    /* <save_dir>/levels/<levelKey>.json
     *
     * {
     *   "deaths": {
     *     "1": 8,
     *     "7": 12,
     *     "24": 2,
     *     ...
     *   },
     *
     *   "runs": {
     *     "2-7": 2,
     *     "23-67": 5,
     *     ...
     *   },
     *
     *   "newBests": [1, 24, 32, ...],
     *   "currentBest": 32,
     *
     *   "sessions": [{
     *     "lastPlayed": 123456789,
     *     "deaths": {...},
     *     "runs": {...},
     *     "newBests": [...],
     *     "currentBest": 12
     *   }, {...}, ...],
     * }
     *
    */
}

void StatsManager::saveData(LevelStats stats, GJGameLevel* level) {
    // log::info("StatsManager::saveData()");

    std::string levelKey = StatsManager::getLevelKey(level);
    if (levelKey == "-1") return;
    
    if (level == m_level)
        m_levelStats = stats;

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath(level);

    // create the json file if it doesnt exist
    if (!ghc::filesystem::exists(levelSaveFilePath)) {
        std::ofstream levelSaveFile(levelSaveFilePath);
        levelSaveFile.close();
    }

    // save the data
    auto indentation = Dev::MINIFY_SAVE_FILE
        ? matjson::NO_INDENTATION
        : 4;

    auto jsonStr = matjson::Value(stats).dump(indentation);
    auto _ = file::writeString(levelSaveFilePath, jsonStr);
}

LevelStats StatsManager::loadData(GJGameLevel* level) {
    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath(level);
    // log::info("StatsManager::loadData() --\n{}", levelSaveFilePath);

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
    int currentSessionBest = -1;

    for (int percent = 0; percent < old__deaths.size(); percent++) {
        auto percentKey = StatsManager::toPercentKey(percent);

        if (old__deaths[percent] > 0)
            deaths[percentKey] = old__deaths[percent];
    }

    for (int percent = 0; percent < old__sessionDeaths.size(); percent++) {
        auto percentKey = StatsManager::toPercentKey(percent);

        if (old__sessionDeaths[percent] > 0)
            sessionDeaths[percentKey] = old__sessionDeaths[percent];

        // some older versions of v1.x.x do not
        // have session best tracking
        if (!old__sessionBests.size()) continue;

        if (old__sessionBests[percent]) {
            sessionBests.insert(percent);
            if (percent > currentSessionBest) currentSessionBest = percent;
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
    int currentBest = 0;

    if (level->isPlatformer()) {
        for (int checkpt = 0; checkpt < old__platBests.size(); checkpt++) {

            if (old__platBests[checkpt]) {
                newBests.insert(checkpt);
                if (checkpt > currentBest) currentBest = checkpt;
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
        for (const auto percent : levelStats.newBests) {
            if (percent == 100) continue;
            auto percentKey = StatsManager::toPercentKey(percent);
            levelStats.deaths[percentKey] = 1;
        }
    }

    return levelStats;
}

std::tuple<NewBests, int> StatsManager::calcNewBests(GJGameLevel* level) {
    NewBests newBests{};
    std::stringstream bestsStream(level->m_personalBests);
    std::string currentBest;
    int currentPercent = 0;

    while (getline(bestsStream, currentBest, ',')) {
        currentPercent += std::stoi(currentBest);
        newBests.insert(currentPercent);
    }

    return std::make_tuple(newBests, currentPercent);
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

ccColor3B StatsManager::inverseColor(ccColor3B color){
    return {static_cast<GLubyte>(abs(color.r - 255)), static_cast<GLubyte>(abs(color.g - 255)), static_cast<GLubyte>(abs(color.b - 255))};
}

std::string StatsManager::getFont(int fontID){
    for (int i = 0; i < m_AllFontsMap.size(); i++)
    {
        if (i == fontID)
            return m_AllFontsMap[i];
    }
    return "-1";
}

std::string StatsManager::getFontName(int fontID){
    for (int i = 0; i < m_AllFontsMap.size(); i++)
    {
        if (i == fontID){
            std::string fontName = "";

            if (i == 0)
                fontName = "Big Font";
            else if (i == 1)
                fontName = "Chat Font";
            else if (i == 2)
                fontName = "Gold Font";
            else{
                fontName = "Font " + std::to_string(i - 2);
            }

            return fontName;
        }
    }
    return "-1";
}

std::vector<std::string> StatsManager::getAllFont(){
    return m_AllFontsMap;
}