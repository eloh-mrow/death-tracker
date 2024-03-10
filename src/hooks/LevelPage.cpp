#include <Geode/modify/LevelPage.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelPage) {
	static void onModify(auto& self) {
		self.setHookPriority("LevelPage::onInfo", -9999);
	}

	void onInfo(CCObject* sender) {
		// only show for actual main levels
		if (m_level->m_levelID.value() > 0)
			DTPopupManager::setInfoAlertOpen(true);

		LevelPage::onInfo(sender);
	}
};