#include <matjson/stl_serialize.hpp>
#include "SaveManager.hpp"
#include "DTPopupManager.hpp"

std::string LEVEL_COUNT_KEY = "levelCount";

GJGameLevel* SaveManager::m_level = nullptr;
int SaveManager::m_levelCount = Mod::get()->getSavedValue<int>(LEVEL_COUNT_KEY, 0);
Deaths SaveManager::m_deaths{};
Deaths SaveManager::m_sessionDeaths{};
NewBests SaveManager::m_sessionBests{};
int SaveManager::m_currentSessionBest = 0;
Progresses SaveManager::m_progresses{};
int SaveManager::m_checkpoint = 0;
bool SaveManager::m_usingStartpos = false;
bool SaveManager::m_shouldResetSessionDeaths = false;
float SaveManager::m_respawnPercent = 0;
std::map<std::string, bool> SaveManager::m_playedLevels{};

// TODO: add backups
void SaveManager::createBackup() {}

std::string SaveManager::getLevelId(GJGameLevel* level) {
	if (level == nullptr) level = m_level; // default to m_level if no parameter specified
	if (level == nullptr) return "";

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

void SaveManager::calcDeathsAndProgresses() {
	if (m_level == nullptr) return;

	// calculate deaths
	auto levelId = SaveManager::getLevelId();
	auto deaths = Mod::get()->getSavedValue<Deaths>(levelId);
	auto sessionDeaths = Mod::get()->getSavedValue<Deaths>(levelId + "-session");
	auto sessionBests = Mod::get()->getSavedValue<NewBests>(levelId + "-session-bests");

	m_deaths = deaths;
	m_sessionDeaths = sessionDeaths;

	// initialize sessionBests
	if (sessionBests.empty())
		sessionBests = NewBests(sessionDeaths.size(), false);

	m_sessionBests = sessionBests;

	// calculate current session best
{
	int percentOrCheckpoint = 0;

	for (auto& count : sessionDeaths) {
		if (count == 0) continue;

		if (percentOrCheckpoint > m_currentSessionBest)
			m_currentSessionBest = percentOrCheckpoint;
	}
}

	// platformer should start empty
	if (SaveManager::isPlatformer()) return;

	// calculate progresses
	// derived from cvolton.betterinfo
	std::string personalBests = m_level->m_personalBests;
	int percentage = m_level->m_newNormalPercent2;

	Progresses progresses{};
	std::stringstream bestsStream(personalBests);
	std::string currentBest;
	int currentPercent = 0;

	while (getline(bestsStream, currentBest, ',')) {
		currentPercent += std::stoi(currentBest);
		progresses[currentPercent] = true;
	}

	// default deaths to progresses x1
	auto pbs = std::string(m_level->m_personalBests);

	if (deaths.empty() && pbs != "" && pbs != "100") {
		deaths = Deaths(100);

		for (auto& [percent, _] : progresses) {
			if (percent == 100) continue; // can't die at 100%
			deaths[percent] = 1;
		}
	}

	m_progresses = progresses;
	m_deaths = deaths;
}

GJGameLevel* SaveManager::getLevel() {
	return m_level;
}

void SaveManager::setLevel(GJGameLevel* level) {
	m_level = level;

	if (level != nullptr) {
		SaveManager::calcDeathsAndProgresses();
	} else {
		m_progresses = Progresses();
		m_deaths = Deaths();
		m_sessionDeaths = Deaths();
	}
}

bool SaveManager::isPlatformer() {
	if (m_level == nullptr) return false;
	return m_level->isPlatformer();
}

bool SaveManager::isNewBest(int percent) {
	if (DTPopupManager::showSessionDeaths())
		return m_sessionBests[percent];

	return m_progresses[percent];
}

void SaveManager::incLevelCount() {
	Mod::get()->setSavedValue(LEVEL_COUNT_KEY, ++m_levelCount);

	// create new backup
	// every 50 levels
	if (m_levelCount % 50 == 0)
		SaveManager::createBackup();
}

void SaveManager::setShouldResetSessionDeaths(bool shouldReset) {
	m_shouldResetSessionDeaths = shouldReset;
}

void SaveManager::resetSessionDeaths() {
	m_shouldResetSessionDeaths = false;
	if (m_level == nullptr) return;

	m_sessionDeaths = Deaths();
	m_sessionBests = NewBests();
	m_currentSessionBest = 0;
	auto levelId = SaveManager::getLevelId();
	Mod::get()->setSavedValue(levelId + "-session", m_sessionDeaths);
	Mod::get()->setSavedValue(levelId + "-session-bests", m_sessionBests);
}

Deaths SaveManager::getDeaths() {
	return DTPopupManager::showSessionDeaths()
		? m_sessionDeaths
		: m_deaths;
}

bool SaveManager::hasNoDeaths(bool checkSessionDeaths) {
	if (m_level == nullptr) return true;

	return checkSessionDeaths && DTPopupManager::showSessionDeaths()
		? m_sessionDeaths.empty()
		: m_deaths.empty();
}

void SaveManager::addDeath(int percent) {
	// new unplayed level
	if (m_deaths.empty()) {
		SaveManager::incLevelCount();

		if (!SaveManager::isPlatformer())
			m_deaths = Deaths(100);
	}

	// reset prev session
	if (m_shouldResetSessionDeaths)
		SaveManager::resetSessionDeaths();

	// no prev session
	if (m_sessionDeaths.empty() && !SaveManager::isPlatformer()) {
		m_sessionDeaths = Deaths(100);
		m_sessionBests = NewBests(100);
	}

	if (SaveManager::isPlatformer()) {
		SaveManager::allocateDeathsForCheckpoint();

		m_deaths[m_checkpoint]++;
		m_sessionDeaths[m_checkpoint]++;

		// calculate session new bests
		if (m_checkpoint > m_currentSessionBest) {
			m_currentSessionBest = m_checkpoint;
			m_sessionBests[m_checkpoint] = true;
		}
	} else {
		m_deaths[percent]++;
		m_sessionDeaths[percent]++;

		// calculate session new bests
		if (percent > m_currentSessionBest) {
			m_currentSessionBest = percent;
			m_sessionBests[percent] = true;
		}
	}

	auto levelId = SaveManager::getLevelId();
	Mod::get()->setSavedValue(levelId, m_deaths);
	Mod::get()->setSavedValue(levelId + "-session", m_sessionDeaths);
	Mod::get()->setSavedValue(levelId + "-session-bests", m_sessionBests);
}

void SaveManager::allocateDeathsForCheckpoint() {
	if (m_level == nullptr) return;

	while (m_checkpoint >= m_deaths.size())
		m_deaths.push_back(0);

	while (m_checkpoint >= m_sessionDeaths.size()) {
		m_sessionDeaths.push_back(0);
		m_sessionBests.push_back(false);
	}
}

void SaveManager::incCheckpoint() {
	m_checkpoint++;
}

void SaveManager::resetCheckpoint() {
	m_checkpoint = 0;
}

bool SaveManager::isUsingStartpos() {
	return m_usingStartpos;
}

void SaveManager::setUsingStartpos(bool usingStartpos) {
	m_usingStartpos = usingStartpos;
}

float SaveManager::getRespawnPercent() {
	return m_respawnPercent;
}

void SaveManager::setRespawnPercent(float respawnPercent) {
	m_respawnPercent = respawnPercent;
}

bool SaveManager::hasPlayedLevel(std::string levelId) {
	return m_playedLevels[levelId];
}

void SaveManager::setPlayedLevel(std::string levelId) {
	m_playedLevels[levelId] = true;
}

long long SaveManager::getLevelSessionTime(std::string levelId) {
	return Mod::get()->getSavedValue<long long>(levelId + "-session-time", -1);
}

void SaveManager::setLevelSessionTime(std::string levelId, long long time) {
	Mod::get()->setSavedValue(levelId + "-session-time", time);
}