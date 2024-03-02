#include <Geode/modify/LevelInfoLayer.hpp>
#include "../utils/SaveManager.hpp"
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	static void onModify(auto& self) {
		self.setHookPriority("LevelInfoLayer::onLevelInfo", -9999);
	}

	void onLevelInfo(CCObject* sender) {
		DTPopupManager::enableDTBtn();
		SaveManager::setLevel(m_level);
		LevelInfoLayer::onLevelInfo(sender);
	}
};