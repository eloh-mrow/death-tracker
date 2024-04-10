#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class $modify(myLevelInfoLayer, LevelInfoLayer) {
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelInfoLayer::onLevelInfo", -9999);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        DTPopupManager::setCurrentLevel(m_level);
        LevelInfoLayer::onLevelInfo(sender);
    }

    bool init(GJGameLevel* p0, bool p1){
        if (!LevelInfoLayer::init(p0, p1)) return false;

        auto s = CCSprite::create("dt_skullBtn.png"_spr);
        s->setScale(0.3f);
        auto btn = CCMenuItemSpriteExtra::create(
            s,
            nullptr,
            this,
            menu_selector(myLevelInfoLayer::openDTLayer)
        );
        btn->setPosition({-175, 0});
        btn->setZOrder(1);

        this->m_playBtnMenu->addChild(btn);

        return true;
    }

    void openDTLayer(CCObject*){
        auto dtLayer = DTLayer::create(this->m_level);
        this->addChild(dtLayer);
    }   
};