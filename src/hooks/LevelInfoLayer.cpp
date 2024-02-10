#include <Geode/modify/LevelInfoLayer.hpp>
#include "../utils/SaveManager.hpp"
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	void onLevelInfo(CCObject* sender) {
		DTPopupManager::enableDTBtn();
		SaveManager::setLevel(m_level);
		LevelInfoLayer::onLevelInfo(sender);
	}
};