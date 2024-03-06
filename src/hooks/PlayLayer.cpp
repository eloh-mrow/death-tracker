#include <Geode/modify/PlayLayer.hpp>
#include "../utils/SaveManager.hpp"

using namespace geode::prelude;

long long getNowSeconds() {
	using namespace std::chrono;
	auto now = system_clock::now();
	return time_point_cast<seconds>(now).time_since_epoch().count();
}

class $modify(PlayLayer) {
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;

		auto levelId = SaveManager::getLevelId(level);
		auto sessionLength = Mod::get()->getSettingValue<int64_t>("session-length");

		switch (sessionLength) {
			// reset session per level play
			case -2: {
				SaveManager::setShouldResetSessionDeaths(true);
				break;
			}

			// reset session per game launch
			case -1: {
				if (SaveManager::hasPlayedLevel(levelId)) break;

				SaveManager::setPlayedLevel(levelId);
				SaveManager::setShouldResetSessionDeaths(true);
				break;
			}

			// reset session based on duration since exited level
			default: {
				auto start = SaveManager::getLevelSessionTime(levelId);
				auto end = getNowSeconds();

				// only reset if the time between session start and now
				// is longer than the max session length
				if (end - start > sessionLength)
					SaveManager::setShouldResetSessionDeaths(true);

				break;
			}
		}

		return true;
	}

	TodoReturn checkpointActivated(CheckpointGameObject* checkpt) {
		PlayLayer::checkpointActivated(checkpt);

		if (m_isPracticeMode) return;
		SaveManager::incCheckpoint();
	}

	TodoReturn resetLevelFromStart() {
		PlayLayer::resetLevelFromStart();
		SaveManager::resetCheckpoint();
	}

	void onQuit() {
		PlayLayer::onQuit();
		SaveManager::resetCheckpoint();
		SaveManager::setShouldResetSessionDeaths(false);

		// set the start time for this session
		auto sessionLength = Mod::get()->getSettingValue<int64_t>("session-length");
		if (sessionLength < 0) return;

		auto levelId = SaveManager::getLevelId(this->m_level);
		auto seconds = getNowSeconds();

		SaveManager::setLevelSessionTime(levelId, seconds);
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		SaveManager::setRespawnPercent(PlayLayer::getCurrentPercent());
		SaveManager::setHasRespawned(true);
	}
};