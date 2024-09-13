#include "../layers/RunAllowedCell.hpp"
#include "../layers/DTLayer.hpp"
#include "../layers/DTLevelSpecificSettingsLayer.hpp"

RunAllowedCell* RunAllowedCell::create(int Precent, float scale, CCNode* DTLayer, CCNode* DTSSLayer) {
    auto ret = new RunAllowedCell();
    if (ret && ret->init(Precent, scale, DTLayer, DTSSLayer)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RunAllowedCell::init(int Precent, float scale, CCNode* DTLayer, CCNode* DTSSLayer){

    buttonSprite = ButtonSprite::create((std::to_string(Precent) + "%").c_str(), "goldFont.fnt", "GJ_button_04.png", 1);
    buttonSprite->setScale(scale);
    if (!CCMenuItemSpriteExtra::init(buttonSprite, nullptr, this, menu_selector(RunAllowedCell::DeleteMe))) return false;

    m_Precent = Precent;
    m_DTLayer = DTLayer;
    m_DTSSLayer = DTSSLayer;

    return true;
}

void RunAllowedCell::DeleteMe(CCObject*){

    static_cast<DTLevelSpecificSettingsLayer*>(m_DTSSLayer)->removeRunAllowed(m_Precent, this);
}
