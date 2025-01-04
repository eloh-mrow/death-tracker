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
    int hideRunLength;
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
            GEODE_UNWRAP_INTO(stats.LinkedLevels, value["LinkedLevels"].as<std::vector<std::string>>());
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

        if (value.contains("hideRunLength")){
            GEODE_UNWRAP_INTO(stats.hideRunLength, value["hideRunLength"].asInt());
        }
        else
            stats.hideRunLength = 0;

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
            { "hideRunLength", value.hideRunLength },
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

    //saves the data based on the last level loaded
    static void saveData();
    //loads the data of the level given
    static Result<LevelStats> loadData(GJGameLevel* const& level);
    //gets new bests from level
    static Result<std::tuple<NewBests, int>> calcNewBests(GJGameLevel* const& level);
    static std::array<std::string, 62> m_AllFontsMap;

public:
    StatsManager() = delete;

    static int MainLevelIDs[26];

    static std::filesystem::path m_savesFolderPath;

    // main functions

    //loads the level stats for a specific level
    static void loadLevelStats(GJGameLevel* const& level);
    //load and retrieve the stats for a level
    static LevelStats getLevelStats(GJGameLevel* const& level);
    //get level stats by a path to the levels save file
    static Result<LevelStats> getLevelStats(const std::filesystem::path& level);
    //get level stats by a levels key
    static Result<LevelStats> getLevelStats(const std::string& levelKey);
    //save a normal mode death to the loaded levels save file
    static void logDeath(const int& percent);
    //save an array of normal mode deaths to the loaded levels save file
    static void logDeaths(const std::vector<int>& percents);
    //save run to the loaded levels save file
    static void logRun(const Run& run);
    //save an array of run to the loaded levels save file
    static void logRuns(const std::vector<Run>& runs);

    // utility functions

    //gets the epoch time
    static long long getNowSeconds();
    //get the a levels level key
    static Result<std::string> getLevelKey(GJGameLevel* const& level = m_level);
    //convert a run string into a run struct
    static Run splitRunKey(const std::string& runKey);
    //get the current ongoing session
    static Session* getSession();
    //update this sessions last played time
    static void updateSessionLastPlayed(const bool& save = false);
    //schedule the creation of a new session
    static void scheduleCreateNewSession(const bool& scheduled);
    //if youve played the loaded level before, returns true
    static bool hasPlayedLevel();
    //gets the path to a levels deaths save file
    static Result<std::filesystem::path> getLevelSaveFilePath(GJGameLevel* const& level = m_level);
    //get a font by a fontID
    static std::string getFont(const int& fontID);
    //get a fonts name by a fontID
    static std::string getFontName(const int& fontID);
    //get an array of all fonts
    static std::array<std::string, 62> getAllFonts();
    //save data to a specific level
    //@param stats data to save
    //@param level level to save data for
    static void saveData(const LevelStats& stats, GJGameLevel* const& level);
    //save data to a specific level
    //@param stats data to save
    //@param levelKey a level key of the level to save data for
    static void saveData(const LevelStats& stats, const std::string& levelKey);
    //gets an array of all levels you have progress on
    static Result<std::vector<std::pair<std::string, LevelStats>>> getAllLevels();
    //seperate a level key to [levelID, levelType]
    static std::pair<std::string, std::string> splitLevelKey(const std::string& levelKey);
    //saves a backup for a specific level
    static void saveBackup(const LevelStats& stats, GJGameLevel* const& level);
    //gets data from a backup of a level
    static Result<LevelStats> getBackupStats(GJGameLevel* const& level);
    //get the difficulty of a level
    //
    // -1 = auto, 0 = NA, 1 = Easy, 2 = Normal
    // 3 = Hard, 4 = Harder, 5 = Insane, 6 = Hard Demon
    // 7 = Easy Demon, 8 = Medium Demon, 9 = Insane Demon, 10 = Extreme Demon
    static int getDifficulty(GJGameLevel* const& level);

    //sets the current deaths save folder path
    static void setPath(const std::filesystem::path& path);

    // return a splitted version of the string provided, devided by the delim
    static std::vector<std::string> splitStr(const std::string& str, const std::string& delim);

    //KMP search

    static void computeLPSArray(const std::string& pat, int M, std::vector<int>& lps);
    static std::vector<int> KMPSearch(const std::string& pat, const std::string& txt);
    
    //gets the index in which the CCLabelBMFonts cursor is located on the string
    static int getCursorPosition(CCLabelBMFont* const& text, CCLabelBMFont* const& cursor);

    //get if a key is in an index of a string
    //@param s the string to look for the key in
    //@param index the index where to look for the key in
    //@param key the key to look for in the string
    static bool isKeyInIndex(const std::string& s, const int& Index, const std::string& key);

    //better info calc :)
    //converts time to a working time string
    static std::string workingTime(long long value);
};