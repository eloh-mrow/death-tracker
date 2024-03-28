#include <Geode/modify/EditLevelLayer.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(EditLevelLayer) {
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("EditLevelLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        DTPopupManager::setCurrentLevel(m_level);
        EditLevelLayer::onLevelInfo(sender);
    }
};