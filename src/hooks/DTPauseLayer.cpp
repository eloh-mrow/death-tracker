#include "../hooks/DTPauseLayer.hpp"

void DTPauseLayer::customSetup(){
    PauseLayer::customSetup();

    if (!Settings::getPauseMenuEnabled()) return;

    auto sideMenu = this->getChildByID("left-button-menu");

    auto s = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
    auto s2 = CCSprite::createWithSpriteFrameName("miniSkull_001.png");
    s2->setPosition(s->getContentSize() / 2);
    s->addChild(s2);
    s->setScale(0.75f);
    auto button = CCMenuItemSpriteExtra::create(
        s,
        nullptr,
        this,
        menu_selector(DTPauseLayer::onDTMiniMenuOpened)
    );

    button->setID("dt-skull-button");
    sideMenu->addChild(button);
    sideMenu->updateLayout();
}

void DTPauseLayer::onDTMiniMenuOpened(CCObject*){
    auto dtLayer = DTLayer::create(DTPopupManager::getCurrentLevel());
    this->addChild(dtLayer);
}

#if defined(GEODE_IS_MACOS)

void DTPauseLayer::onNormalMode(cocos2d::CCObject* sender){
    PauseLayer::onNormalMode(sender);

    static_cast<DTPlayLayer*>(GameManager::get()->getPlayLayer())->m_fields->currentRun.end = 0;

    //log::info("PauseLayer::onNormalMode()");
}

#endif