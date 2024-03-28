#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelInfoLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        DTPopupManager::setCurrentLevel(m_level);
        LevelInfoLayer::onLevelInfo(sender);
    }
};