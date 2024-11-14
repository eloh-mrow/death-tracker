#pragma once

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;

typedef std::map<std::string, int> Deaths;
typedef std::map<std::string, int> Runs;
typedef std::set<int> NewBests;

struct Run_s{
    int start;
    int end;

    Run_s(int _start, int _end) : start(_start), end(_end){}

    Run_s() = default;
};
typedef struct Run_s Run; 

struct DeathInfo_s{
    Run run;
    bool isNewBest;
    int deaths;
    float passrate;

    DeathInfo_s(Run _run, bool _isNewBest, int _deaths, float _passrate) : run(_run), isNewBest(_isNewBest), deaths(_deaths), passrate(_passrate){}
    DeathInfo_s(Run _run, int _deaths, float _passrate) : run(_run), deaths(_deaths), passrate(_passrate){}
};
typedef struct DeathInfo_s DeathInfo;

typedef struct {
    long long lastPlayed;
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    int currentBest;
    long long sessionStartDate;
} Session;

struct LevelStats_s {
    Deaths deaths;
    Runs runs;
    NewBests newBests;
    int currentBest;
    std::vector<Session> sessions;
    std::vector<int> RunsToSave{-1};
    std::vector<std::string> LinkedLevels;
    std::string levelName = "Unknown name";
    int attempts;
    int difficulty;
    int hideUpto;
};
typedef struct LevelStats_s LevelStats;

template <>
struct matjson::Serialize<Session> {
    static Result<Session> fromJson(const matjson::Value& value) {
        Session session;
        GEODE_UNWRAP_INTO(session.lastPlayed, value["lastPlayed"].as<long long>());
        GEODE_UNWRAP_INTO(session.deaths, value["deaths"].as<Deaths>());
        GEODE_UNWRAP_INTO(session.runs, value["runs"].as<Runs>());
        GEODE_UNWRAP_INTO(session.newBests, value["newBests"].as<NewBests>());
        GEODE_UNWRAP_INTO(session.currentBest, value["currentBest"].asInt());

        if (value.contains("sessionStartDate")){
            GEODE_UNWRAP_INTO(session.sessionStartDate, value["sessionStartDate"].as<long long>());
        }
        else
            session.sessionStartDate = -1;

        return Ok(session);
    }

    static matjson::Value toJson(const Session& value) {
        matjson::Value obj = matjson::makeObject({
            { "lastPlayed", value.lastPlayed },
            { "deaths", value.deaths },
            { "runs", value.runs },
            { "newBests", value.newBests },
            { "currentBest", value.currentBest },
            { "sessionStartDate", value.sessionStartDate },
        });
        return obj;
    }
};

template <>
struct matjson::Serialize<LevelStats> {
    static Result<LevelStats> fromJson(const matjson::Value& value) {

        LevelStats stats;
        GEODE_UNWRAP_INTO(stats.deaths, value["deaths"].as<Deaths>());
        GEODE_UNWRAP_INTO(stats.runs, value["runs"].as<Runs>());
        GEODE_UNWRAP_INTO(stats.newBests, value["newBests"].as<NewBests>());
        GEODE_UNWRAP_INTO(stats.currentBest, value["currentBest"].asInt());
        GEODE_UNWRAP_INTO(stats.sessions, value["sessions"].as<std::vector<Session>>());

        if (value.contains("RunsToSave")){
            GEODE_UNWRAP_INTO(stats.RunsToSave, value["RunsToSave"].as<std::vector<int>>());
        }

        if (value.contains("LinkedLevels")){
            GEODE_UNWRAP_INTO(stats.RunsToSave, value["RunsToSave"].as<std::vector<int>>());
        }
        else
            stats.LinkedLevels = value["LinkedLevels"].as<std::vector<std::string>>().unwrapOr(std::vector<std::string>{});
        
        if (value.contains("levelName")){
            GEODE_UNWRAP_INTO(stats.levelName, value["levelName"].asString());
        }
        else
            stats.levelName = "-1";

        if (value.contains("attempts")){
            GEODE_UNWRAP_INTO(stats.attempts, value["attempts"].asInt());
        }
        else
            stats.attempts = -1;

        if (value.contains("difficulty")){
            GEODE_UNWRAP_INTO(stats.difficulty, value["difficulty"].asInt());
        }
        else
            stats.difficulty = 0;

        if (value.contains("hideUpto")){
            GEODE_UNWRAP_INTO(stats.hideUpto, value["hideUpto"].asInt());
        }
        else
            stats.hideUpto = 0;

        return Ok(stats);
    }

    static matjson::Value toJson(const LevelStats& value) {
        matjson::Value obj = matjson::makeObject({
            { "deaths", value.deaths },
            { "runs", value.runs },
            { "newBests", value.newBests },
            { "currentBest", value.currentBest },
            { "sessions", value.sessions },
            { "RunsToSave", value.RunsToSave },
            { "LinkedLevels", value.LinkedLevels },
            { "levelName", value.levelName },
            { "attempts", value.attempts },
            { "difficulty", value.difficulty },
            { "hideUpto", value.hideUpto },
        });
        return obj;
    }
};

typedef struct {
    std::string name;
    float value;
    std::string type;
} prismSetting;

// matjson fuckery
template <>
struct matjson::Serialize<prismSetting> {
    static Result<prismSetting> fromJson(const matjson::Value& value) {
        prismSetting setting;
        GEODE_UNWRAP_INTO(setting.name, value["name"].asString());
        setting.value = 0;
        GEODE_UNWRAP_INTO(setting.type, value["type"].asString());

        if (value["value"].isBool()){
            GEODE_UNWRAP_INTO(auto valRes, value["value"].asBool());
            setting.value = valRes ? 1 : 0;
        }
        else{
            GEODE_UNWRAP_INTO(setting.value, value["value"].asDouble());
        }

        return Ok(setting);
    }
};

class StatsManager {
private:
    static GJGameLevel* m_level;
    static std::set<std::string> m_playedLevels;

    static LevelStats m_levelStats;
    
    static bool m_scheduleCreateNewSession;

    // internal functions
    static void saveData();
    static Result<LevelStats> loadData(GJGameLevel* level);
    static Result<std::tuple<NewBests, int>> calcNewBests(GJGameLevel* level);
    static std::array<std::string, 62> m_AllFontsMap;

public:
    StatsManager() = delete;

    static int MainLevelIDs[26];

    static std::filesystem::path m_savesFolderPath;

    // main functions
    static void loadLevelStats(GJGameLevel* level);
    static LevelStats getLevelStats(GJGameLevel* level);
    static Result<LevelStats> getLevelStats(const std::filesystem::path& level);
    static Result<LevelStats> getLevelStats(const std::string& levelKey);
    static void logDeath(int percent);
    static void logDeaths(const std::vector<int>& percents);
    static void logRun(const Run run);
    static void logRuns(const std::vector<Run>& runs);

    // utility functions
    static long long getNowSeconds();
    static Result<std::string> getLevelKey(GJGameLevel* level = m_level);
    static Run splitRunKey(const std::string& runKey);
    static Session* getSession();
    static void updateSessionLastPlayed(bool save = false);
    static void scheduleCreateNewSession(bool scheduled);
    static bool hasPlayedLevel();
    static Result<std::filesystem::path> getLevelSaveFilePath(GJGameLevel* level = m_level);
    static bool ContainsAtIndex(int startIndex, const std::string& check, const std::string& str);
    static ccColor3B inverseColor(ccColor3B color);
    static std::string getFont(int fontID);
    static std::string getFontName(int fontID);
    static std::array<std::string, 62> getAllFont();
    static void saveData(const LevelStats& stats, GJGameLevel* level);
    static void saveData(const LevelStats& stats, const std::string& levelKey);
    static Result<std::vector<std::pair<std::string, LevelStats>>> getAllLevels();
    static std::pair<std::string, std::string> splitLevelKey(const std::string& levelKey);
    static void saveBackup(const LevelStats& stats, GJGameLevel* level);
    static Result<LevelStats> getBackupStats(GJGameLevel* level);
    /*
    -1 = auto, 0 = NA, 1 = Easy, 2 = Normal
    3 = Hard, 4 = Harder, 5 = Insane, 6 = Hard Demon
    7 = Easy Demon, 8 = Medium Demon, 9 = Insane Demon, 10 = Extreme Demon
    */
    static int getDifficulty(GJGameLevel* level);

    static void setPath(const std::filesystem::path& path);

    static std::vector<std::string> splitStr(const std::string& str, const std::string& delim);

    static void computeLPSArray(const std::string& pat, int M, std::vector<int>& lps);

    static std::vector<int> KMPSearch(const std::string& pat, const std::string& txt);
};