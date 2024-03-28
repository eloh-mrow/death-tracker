#include <Geode/modify/FLAlertLayer.hpp>
#include "../layers/DTPopup.hpp"
#include "../managers/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(MyFLAlertLayer, FLAlertLayer) {
    CCMenu* m_dtBtnMenu = nullptr;

    bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
        if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8))
            return false;

        if (!DTPopupManager::isInfoAlertOpen()) return true;

        auto alertBgSize = getChildOfType<CCScale9Sprite>(m_mainLayer, 0)->getContentSize();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto btnSpr = CCSprite::create("dt_skullBtn.png"_spr);
        btnSpr->setScale(0.25f);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSpr,
            this,
            menu_selector(MyFLAlertLayer::openPopup)
        );

        btn->setPosition({
            winSize.width / 2 + alertBgSize.width / 2 - btn->getContentSize().width / 2 - 13,
            winSize.height / 2 - alertBgSize.height / 2 + btn->getContentSize().height / 2 + 13
        });

        auto menu = CCMenu::create();

        menu->setZOrder(100);
        menu->setPosition({0,0});

        menu->addChild(btn);
        m_mainLayer->addChild(menu);
        m_fields->m_dtBtnMenu = menu;

        handleTouchPriority(this);
        return true;
    }

    void show() {
        FLAlertLayer::show();

        // compatibility with older version of better info
        if (!DTPopupManager::isInfoAlertOpen()) return;
        if (!Loader::get()->isModLoaded("cvolton.betterinfo")) return;
        if (this->getChildByIDRecursive("cvolton.betterinfo/next-button")) return;

        // force remove the button
        DTPopupManager::setInfoAlertOpen(false);

        if (m_fields->m_dtBtnMenu) {
            m_fields->m_dtBtnMenu->removeFromParent();
            m_fields->m_dtBtnMenu = nullptr;
        }
    }

    void onBtn1(CCObject* sender) {
        FLAlertLayer::onBtn1(sender);

        if (m_button1->getParent() == sender)
            DTPopupManager::setInfoAlertOpen(false);
    }

    void keyBackClicked() {
        FLAlertLayer::keyBackClicked();
        DTPopupManager::setInfoAlertOpen(false);
    }

    void openPopup(CCObject* sender) {
        m_mainLayer->setVisible(false);

        auto dtPopup = DTPopup::create(160, 240, this, DTPopupManager::getCurrentLevel());
        this->addChild(dtPopup);
    }
};