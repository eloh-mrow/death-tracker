#include <Geode/modify/EditLevelLayer.hpp>
#include "../utils/SaveManager.hpp"
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(EditLevelLayer) {
	static void onModify(auto& self) {
		self.setHookPriority("EditLevelLayer::onLevelInfo", -9999);
	}

	void onLevelInfo(CCObject* sender) {
		SaveManager::setLevel(m_level);
		DTPopupManager::enableDTBtn();
		EditLevelLayer::onLevelInfo(sender);
	}
};