#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelInfoLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        log::info("did thing");
        LevelInfoLayer::onLevelInfo(sender);
    }

    bool init(GJGameLevel* level, bool p1){
        if (!LevelInfoLayer::init(level, p1)) return false;

        DTPopupManager::setCurrentLevel(level);
    }
};