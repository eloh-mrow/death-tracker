#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        self.setHookPriority("LevelInfoLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        LevelInfoLayer::onLevelInfo(sender);
    }
};