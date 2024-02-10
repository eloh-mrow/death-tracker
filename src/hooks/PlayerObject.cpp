#include <Geode/modify/PlayerObject.hpp>
#include "../utils/SaveManager.hpp"

using namespace geode::prelude;

class $modify(PlayerObject) {
	TodoReturn playerDestroyed(bool p0) {
		PlayerObject::playerDestroyed(p0);

		// disable in editor
		auto playLayer = PlayLayer::get();
		if (playLayer == nullptr) return;
		auto level = playLayer->m_level;

		// disable in practice
		if (playLayer->m_isPracticeMode) return;

		// platformer + startpos = no worky, incorrect death tracking
		if (SaveManager::isPlatformer() && SaveManager::isUsingStartpos()) return;

		// disable tracking deaths on completed levels
		if (
			Mod::get()->getSettingValue<bool>("disable-tracking-completed-levels")
			&& level->m_newNormalPercent2.value() == 100
			&& level->m_levelType == GJLevelType::Saved
		)
			return;

		// save deaths
		if (SaveManager::getLevel() != level)
			SaveManager::setLevel(level);

		SaveManager::addDeath(playLayer->getCurrentPercentInt());
	}
};