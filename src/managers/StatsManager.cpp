#include "StatsManager.hpp"
#include "../utils/Settings.hpp"
#include "../utils/Dev.hpp"
#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

std::vector<std::string> StatsManager::splitStr(const std::string& str, const std::string& delim) {
    size_t posStart = 0;
    size_t posEnd;
    size_t delimLen = delim.length();

    std::vector<std::string> res;

    while ((posEnd = str.find(delim, posStart)) != std::string::npos) {
        std::string token = str.substr(posStart, posEnd - posStart);
        posStart = posEnd + delimLen;
        if (!token.empty())
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

std::filesystem::path StatsManager::m_savesFolderPath = Settings::getSavePath();

int StatsManager::MainLevelIDs[26]{
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 5001, 5002, 5003, 5004, 3001
};

std::array<std::string, 62> StatsManager::m_AllFontsMap{
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
void StatsManager::loadLevelStats(GJGameLevel* const& level) {
    if (m_level == level) return;

    auto levelStatsRes = StatsManager::loadData(level);
    if (!levelStatsRes.isOk()){
        return;
    }
    auto levelStats = levelStatsRes.unwrap();

    m_levelStats = levelStats;
    m_level = level;
}

LevelStats StatsManager::getLevelStats(GJGameLevel* const& level) {
    StatsManager::loadLevelStats(level);
    return m_levelStats;
}

Result<LevelStats> StatsManager::getLevelStats(const std::filesystem::path& level){
    auto levelSaveFilePath = level;

    if (std::filesystem::exists(levelSaveFilePath)){
        auto res = file::readJson(levelSaveFilePath);
        GEODE_UNWRAP_INTO(auto json, res);
        
        return json.as<LevelStats>();
    }

    return Err("deaths json does not exist!");
}

Result<LevelStats> StatsManager::getLevelStats(const std::string& levelKey){
    auto levelSaveFilePath = m_savesFolderPath / (levelKey + ".json");

    if (std::filesystem::exists(levelSaveFilePath)){
        auto res = file::readJson(levelSaveFilePath);
        GEODE_UNWRAP_INTO(auto json, res);

        return json.as<LevelStats>();
    }

    return Err("deaths json does not exist!");
}

Result<LevelStats> StatsManager::getBackupStats(GJGameLevel* const& level){
    auto levelKeyRes = StatsManager::getLevelKey(level);

    GEODE_UNWRAP_INTO(std::string levelKey, levelKeyRes);

    auto levelSaveFilePath = m_savesFolderPath / (levelKey + ".deathsBackup");

    if (std::filesystem::exists(levelSaveFilePath)){
        auto res = file::readJson(levelSaveFilePath);
        GEODE_UNWRAP_INTO(auto json, res);

        return json.as<LevelStats>();
    }

    return Err("deaths json does not exist!");
}

void StatsManager::logDeath(const int& percent) {
    auto session = StatsManager::getSession();
    if (!session) return;

    auto percentKey = std::to_string(percent);

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

void StatsManager::logDeaths(const std::vector<int>& percents) {
    auto session = StatsManager::getSession();
    if (!session) return;

    for (int i = 0; i < percents.size(); i++)
    {
        auto percentKey = std::to_string(percents[i]);

        m_levelStats.deaths[percentKey]++;
        session->deaths[percentKey]++;

        if (percents[i] > m_levelStats.currentBest) {
            m_levelStats.currentBest = percents[i];
            m_levelStats.newBests.insert(percents[i]);
        }

        if (percents[i] > session->currentBest) {
            session->currentBest = percents[i];
            session->newBests.insert(percents[i]);
        }
    }

    StatsManager::updateSessionLastPlayed();
    StatsManager::saveData();
}

void StatsManager::logRun(const Run& run) {
    bool TrackRun = false;
    if (m_levelStats.RunsToSave.size()){
        if (m_levelStats.RunsToSave[0] == -1){
            TrackRun = true;
        }
        else{
            for (int i = 0; i < m_levelStats.RunsToSave.size(); i++)
            {
                if (m_levelStats.RunsToSave[i] == run.start){
                    TrackRun = true;
                    break;
                }
            }
        }
    }

    if (!TrackRun) return;

    auto session = StatsManager::getSession();
    if (!session) return;

    auto runKey = fmt::format("{}-{}",
        run.start,
        run.end
    );

    m_levelStats.runs[runKey]++;
    session->runs[runKey]++;

    StatsManager::updateSessionLastPlayed();
    StatsManager::saveData();
}

void StatsManager::logRuns(const std::vector<Run>& runs) {
    bool TrackRun = false;
    for (int i = 0; i < runs.size(); i++)
    {
        if (m_levelStats.RunsToSave.size()){
            if (m_levelStats.RunsToSave[0] == -1){
                TrackRun = true;
            }
            else{
                for (int i = 0; i < m_levelStats.RunsToSave.size(); i++)
                {
                    if (m_levelStats.RunsToSave[i] == runs[i].start){
                        TrackRun = true;
                        break;
                    }
                }
            }
        }
            
        if (!TrackRun) return;

        auto session = StatsManager::getSession();
        if (!session) return;

        auto runKey = fmt::format("{}-{}",
            runs[i].start,
            runs[i].end
        );

        m_levelStats.runs[runKey]++;
        session->runs[runKey]++;
    }

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

Result<std::string> StatsManager::getLevelKey(GJGameLevel* const& level) {
	if (!level) return Err("invalid level!");

	std::string levelId;

    if (level->m_levelType == GJLevelType::Editor){
        levelId += std::to_string(EditorIDs::getID(level));
    }
    else{
        levelId += std::to_string(level->m_levelID.value());
    }

	// local level postfix
	if (level->m_levelType == GJLevelType::Local)
		levelId += "-local";

    if (level->m_levelType == GJLevelType::Editor)
		levelId += "-editor";

	// daily/weekly postfix
	if (level->m_dailyID > 0)
		levelId += "-daily";

	// gauntlet level postfix
	if (level->m_gauntletLevel)
		levelId += "-gauntlet";

	return Ok(levelId);
}

Run StatsManager::splitRunKey(const std::string& runKey) {
    auto runKeySplit = StatsManager::splitStr(runKey, "-");

    auto start = std::stof(runKeySplit[0]);
    auto end = std::stof(runKeySplit[1]);

    Run r;
    r.start = start;
    r.end = end;

    return r;
}

Session* StatsManager::getSession() {
    if (m_levelStats.currentBest == -1) return nullptr;

    auto currentSession = &m_levelStats.sessions[m_levelStats.sessions.size() - 1];

    // new sessions can be scheduled
    // and are created when the player dies
    if (!m_scheduleCreateNewSession) return currentSession;
    m_scheduleCreateNewSession = false;

    std::string levelKey = StatsManager::getLevelKey().unwrapOr("-1");

    if (levelKey == "-1"){
        return currentSession;
    }

    // the user has played the level
    // if a new session is created
    m_playedLevels.insert(levelKey);

    // create the new session
    auto session = Session {
        .lastPlayed = -1,
        .deaths = {},
        .runs = {},
        .newBests = {},
        .currentBest = -1,
        .sessionStartDate = StatsManager::getNowSeconds()
    };

    m_levelStats.sessions.push_back(session);
    return &m_levelStats.sessions[m_levelStats.sessions.size() - 1];
}

void StatsManager::updateSessionLastPlayed(const bool& save) {
    auto now = StatsManager::getNowSeconds();
    auto session = StatsManager::getSession();

    session->lastPlayed = now;

    if (save && session->deaths.size() > 0)
        StatsManager::saveData();
}

void StatsManager::scheduleCreateNewSession(const bool& scheduled) {
    if (m_levelStats.currentBest != -1)
        m_scheduleCreateNewSession = scheduled;
}

bool StatsManager::hasPlayedLevel() {
    if (!m_level) return false;

    std::string levelKey = StatsManager::getLevelKey().unwrapOr("-1");

    if (levelKey == "-1"){
        return false;
    }

    return m_playedLevels.contains(levelKey);
}

/* internal functions
======================= */
void StatsManager::saveData() {
    if (m_levelStats.currentBest == -1) return;

    std::string levelKey = StatsManager::getLevelKey().unwrapOr("-1");
    
    if (levelKey == "-1") return;

    std::filesystem::path path{};

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath().unwrapOrDefault();
    if (levelSaveFilePath.empty()) return;

    // create the json file if it doesnt exist
    if (!std::filesystem::exists(levelSaveFilePath)) {
        std::ofstream levelSaveFile(levelSaveFilePath);
        levelSaveFile.close();
    }

    // save the data
    auto indentation = Dev::MINIFY_SAVE_FILE
        ? matjson::NO_INDENTATION
        : 4;

    auto jsonStr = matjson::Value(m_levelStats).dump(indentation);
    auto _ = file::writeString(levelSaveFilePath, jsonStr);
}

void StatsManager::saveData(const LevelStats& stats, GJGameLevel* const& level) {
    std::string levelKey = StatsManager::getLevelKey(level).unwrapOr("-1");
    if (levelKey == "-1") return;
    
    if (level == m_level)
        m_levelStats = stats;

    auto levelSaveFilePath = StatsManager::getLevelSaveFilePath(level).unwrapOrDefault();
    if (levelSaveFilePath.empty()) return;

    // create the json file if it doesnt exist
    if (!std::filesystem::exists(levelSaveFilePath)) {
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

void StatsManager::saveBackup(const LevelStats& stats, GJGameLevel* const& level) {
    std::string levelKey = StatsManager::getLevelKey(level).unwrapOr("-1");
    if (levelKey == "-1") return;

    std::filesystem::path levelSaveFilePath = m_savesFolderPath / (levelKey + ".deathsBackup");

    // create the json file if it doesnt exist
    if (!std::filesystem::exists(levelSaveFilePath)) {
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

void StatsManager::saveData(const LevelStats& stats, const std::string& levelKey) {    
    if (levelKey == StatsManager::getLevelKey(m_level).unwrapOr("-1"))
        m_levelStats = stats;

    auto levelSaveFilePath = m_savesFolderPath / (levelKey + ".json");

    // create the json file if it doesnt exist
    if (!std::filesystem::exists(levelSaveFilePath)) {
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

Result<LevelStats> StatsManager::loadData(GJGameLevel* const& level) {
    GEODE_UNWRAP_INTO(auto levelSaveFilePath, StatsManager::getLevelSaveFilePath(level));

    if (std::filesystem::exists(levelSaveFilePath)){
        auto res = file::readJson(levelSaveFilePath);
        GEODE_UNWRAP_INTO(auto json, res);
        
        return json.as<LevelStats>();
    }
        
    // get defaults for level stats
    // includes backwards compatibility for v1.x.x
    LevelStats levelStats{};

    GEODE_UNWRAP_INTO(auto levelKey, StatsManager::getLevelKey(level));

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
        auto percentKey = std::to_string(percent);

        if (old__deaths[percent] > 0)
            deaths[percentKey] = old__deaths[percent];
    }

    for (int percent = 0; percent < old__sessionDeaths.size(); percent++) {
        auto percentKey = std::to_string(percent);

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
        auto bestsCalcRes = StatsManager::calcNewBests(level);
        if (bestsCalcRes.isOk()){
            const auto [_newBests, _currentBest] = bestsCalcRes.unwrap();
            newBests = _newBests;
            currentBest = _currentBest;
        }
    }

    levelStats.newBests = newBests;
    levelStats.currentBest = currentBest;

    // default deaths to newBests x1
    if (!deaths.size()) {
        for (const auto percent : levelStats.newBests) {
            if (percent == 100) continue;
            auto percentKey = std::to_string(percent);
            levelStats.deaths[percentKey] = 1;
        }
    }

    return Ok(levelStats);
}

Result<std::tuple<NewBests, int>> StatsManager::calcNewBests(GJGameLevel* const& level) {
    NewBests newBests{};
    std::stringstream bestsStream(level->m_personalBests);
    std::string currentBest;
    int currentPercent = 0;

    while (std::getline(bestsStream, currentBest, ',')) {
        GEODE_UNWRAP_INTO(auto tempPercent, geode::utils::numFromString<int>(currentBest));
        
        currentPercent += tempPercent;
        newBests.insert(currentPercent);
    }

    return Ok(std::make_tuple(newBests, currentPercent));
}

Result<std::filesystem::path> StatsManager::getLevelSaveFilePath(GJGameLevel* const& level) {
    std::filesystem::path filePath{};
    GEODE_UNWRAP_INTO(auto levelKey, StatsManager::getLevelKey(level));

    filePath = m_savesFolderPath / (levelKey + ".json");
    return Ok(filePath);
}

std::string StatsManager::getFont(const int& fontID){
    for (int i = 0; i < m_AllFontsMap.size(); i++)
    {
        if (i == fontID)
            return m_AllFontsMap[i];
    }
    return m_AllFontsMap[1];
}

std::string StatsManager::getFontName(const int& fontID){
    for (int i = 0; i < m_AllFontsMap.size(); i++)
    {
        if (i == fontID){
            std::string fontName;

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
    return "Chat Font";
}

std::array<std::string, 62> StatsManager::getAllFonts(){
    return m_AllFontsMap;
}

Result<std::vector<std::pair<std::string, LevelStats>>> StatsManager::getAllLevels(){
    auto res = file::readDirectory(m_savesFolderPath);
    GEODE_UNWRAP_INTO(auto allLevels, res);

    std::vector<std::pair<std::string, LevelStats>> toReturn;

    for (int i = 0; i < allLevels.size(); i++)
    {
        if (allLevels[i].extension().string() == ".json"){
            GEODE_UNWRAP_INTO(auto stats, StatsManager::getLevelStats(allLevels[i]));

            toReturn.push_back(std::make_pair(allLevels[i].stem().string(), stats));
        }
    }
    
    return Ok(toReturn);
}

std::pair<std::string, std::string> StatsManager::splitLevelKey(const std::string& levelKey){
    auto levelKeySplit = splitStr(levelKey, "-");

    std::pair<std::string, std::string> toReturn;

    toReturn.first = levelKeySplit[0];

    if (levelKeySplit.size() == 2){
        toReturn.second = levelKeySplit[1];
    }
    else{
        toReturn.second = "online";
    }

    return toReturn;
}

int StatsManager::getDifficulty(GJGameLevel* const& level){
    if (level->m_autoLevel)
        return -1;

    if (level->m_ratingsSum != 0)
        if (level->m_demon == 1){
            int fixedNum = level->m_demonDifficulty;

            if (fixedNum != 0)
                fixedNum -= 2;

            return 6 + fixedNum;
        }
        else{
            return level->m_ratingsSum / level->m_ratings;
        }
    else 
        return 0;
}

void StatsManager::setPath(const std::filesystem::path& path){
    m_savesFolderPath = path;
}

void StatsManager::computeLPSArray(const std::string& pat, int M, std::vector<int>& lps) {
    int length = 0;
    int i = 1;
    lps[0] = 0;

    while (i < M) {
        if (pat[i] == pat[length]) {
            length++;
            lps[i] = length;
            i++;
        }
        else {
            if (length != 0) {
                length = lps[length - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

std::vector<int> StatsManager::KMPSearch(const std::string& pat, const std::string& txt) {
    std::vector<int> toReturn{};

    int M = pat.size();
    int N = txt.size();
    std::vector<int> lps(M);

    computeLPSArray(pat, M, lps);

    int i = 0;
    int j = 0;
    while (i < N) {
        if (pat[j] == txt[i]) {
            j++;
            i++;
        }

        if (j == M) {
            toReturn.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < N && pat[j] != txt[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }

    return toReturn;
}

int StatsManager::getCursorPosition(CCLabelBMFont* const& text, CCLabelBMFont* const& cursor){
    if (text->getString() == nullptr) return -1;
    if (text->getString()[0] == '\0') return -1;

    std::string tempS = text->getString();
    if (tempS == " ") return 0;

    int index = -1;

    CCObject* child;

    CCARRAY_FOREACH(text->getChildren(), child){
        if (auto node = typeinfo_cast<CCNode*>(child)){
            if (node->isVisible()){
                index++;

                if (node->getParent()->convertToWorldSpace(node->getPosition()).x > cursor->getParent()->convertToWorldSpace(cursor->getPosition()).x)
                    return index;
            }
        }
    }
    
    return index + 1;
}

bool StatsManager::isKeyInIndex(const std::string& s, const int& index, const std::string& key) {
    if (index + key.length() > s.length()) return false;

    return s.substr(index, key.length()) == key;
}

std::string StatsManager::workingTime(long long value){
    if(value < 0) return fmt::format("NA ({})", value);
    if(value == 0) return "NA";

    int hours = value / 3600;
    int minutes = (value % 3600) / 60;
    int seconds = value % 60;

    std::ostringstream stream;
    if(hours > 0) stream << hours << "h ";
    if(minutes > 0) stream << minutes << "m ";
    stream << seconds << "s";

    return stream.str();
}
