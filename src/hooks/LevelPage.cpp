#include <Geode/modify/LevelPage.hpp>
#include "../utils/SaveManager.hpp"
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelPage) {
	static void onModify(auto& self) {
		self.setHookPriority("LevelPage::onInfo", -9999);
	}

	void onInfo(CCObject* sender) {
		// only show for actual main levels
		if (m_level->m_levelID.value() > 0) {
			DTPopupManager::enableDTBtn();
			SaveManager::setLevel(m_level);
		}

		LevelPage::onInfo(sender);
	}
};