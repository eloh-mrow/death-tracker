#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;
typedef std::vector<int> Deaths;
typedef std::map<int, bool> Progresses;

class SaveManager {
private:
	static GJGameLevel* m_level;
	static int m_levelCount;
	static Deaths m_deaths;
	static Deaths m_sessionDeaths;
	static Progresses m_progresses;
	static int m_checkpoint;
	static bool m_usingStartpos;
	static bool m_shouldResetSessionDeaths;
	static float m_respawnPercent;
	static std::map<std::string, bool> m_playedLevels;

	static void createBackup();
	static void calcDeathsAndProgresses();

public:
	SaveManager() = delete;

	static GJGameLevel* getLevel();
	static std::string getLevelId(GJGameLevel* level = nullptr);
	static void setLevel(GJGameLevel* level);
	static bool isPlatformer();
	static bool isNewBest(int percent);
	static void incLevelCount();
	static void setShouldResetSessionDeaths(bool shouldReset);
	static void resetSessionDeaths();
	static Deaths getDeaths();
	static bool hasNoDeaths(bool checkSessionDeaths = false);
	static void addDeath(int percent = 0);
	static void allocateDeathsForCheckpoint();
	static void incCheckpoint();
	static void resetCheckpoint();
	static bool isUsingStartpos();
	static void setUsingStartpos(bool usingStartpos);
	static float getRespawnPercent();
	static void setRespawnPercent(float respawnPercent);
	static bool hasPlayedLevel(std::string levelId);
	static void setPlayedLevel(std::string levelId);
	static long long getLevelSessionTime(std::string levelId);
	static void setLevelSessionTime(std::string levelId, long long time);
};