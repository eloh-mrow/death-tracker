#include <Geode/modify/PlayLayer.hpp>
#include "../utils/SaveManager.hpp"

using namespace geode::prelude;

class $modify(PlayLayer) {
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;
		SaveManager::setShouldResetSessionDeaths();
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

	TodoReturn onQuit() {
		PlayLayer::onQuit();
		SaveManager::resetCheckpoint();
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		SaveManager::setRespawnPercent(PlayLayer::getCurrentPercent());
	}
};