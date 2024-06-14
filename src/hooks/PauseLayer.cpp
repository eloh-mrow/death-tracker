#include <Geode/modify/PauseLayer.hpp>
#include "../layers/DTLayer.hpp"
#include "../managers/DTPopupManager.hpp"
#include "../utils/Settings.hpp"

using namespace geode::prelude;

class $modify(DTPauseLayer, PauseLayer) {

    void customSetup(){
        PauseLayer::customSetup();

        if (!Settings::getPauseMenuEnabled()) return;

        auto sideMenu = this->getChildByID("left-button-menu");

        auto buttonSprite = CCSprite::create("dt_skullBtn.png"_spr);
        buttonSprite->setScale(0.25f);
        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            nullptr,
            this,
            menu_selector(DTPauseLayer::onDTMiniMenuOpened)
        );

        sideMenu->addChild(button);
        sideMenu->updateLayout();
    }

    void onDTMiniMenuOpened(CCObject*){
        auto dtLayer = DTLayer::create(DTPopupManager::getCurrentLevel());
        this->addChild(dtLayer);
    }

};