#include "../layers/DTPopup.hpp"

DTPopup* DTPopup::create(float width, float hight, FLAlertLayer* const& InfoAlertLayer) {
    auto ret = new DTPopup();
    if (ret && ret->init(width, hight, InfoAlertLayer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTPopup::setup(FLAlertLayer* const& InfoAlertLayer) {

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_InfoAlertLayer = InfoAlertLayer;

    return true;
}

void DTPopup::onClose(cocos2d::CCObject* object){
    m_InfoAlertLayer->m_mainLayer->setVisible(true);
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}