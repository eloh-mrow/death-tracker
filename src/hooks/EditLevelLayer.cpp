#include <Geode/modify/EditLevelLayer.hpp>
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(EditLevelLayer) {
    static void onModify(auto& self) {
        self.setHookPriority("EditLevelLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        EditLevelLayer::onLevelInfo(sender);
    }
};