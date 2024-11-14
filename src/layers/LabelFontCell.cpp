#include "../layers/LabelFontCell.hpp"
#include "../layers/LabelSettingsLayer.hpp"

LabelFontCell* LabelFontCell::create(int FontID, std::string FontFNT, std::string FontName, CCNode* SettingsLayer) {
    auto ret = new LabelFontCell();
    if (ret && ret->init(FontID, FontFNT, FontName, SettingsLayer)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LabelFontCell::init(int FontID, std::string FontFNT, std::string FontName, CCNode* SettingsLayer){
    m_FontID = FontID;
    m_FontFNT = FontFNT;
    m_FontName = FontName;
    m_SettingsLayer = SettingsLayer;

    auto label = CCLabelBMFont::create(m_FontName.c_str(), m_FontFNT.c_str());
    label->setScale(0.35f);
    label->setAnchorPoint({0, 0.5f});
    label->setPosition({2, 10});
    this->addChild(label);

    auto bMenu = CCMenu::create();
    bMenu->setPosition({0,0});
    this->addChild(bMenu);

    auto SelectButtonSprite = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
    SelectButtonSprite->setScale(0.35f);
    auto SelectButton = CCMenuItemSpriteExtra::create(
        SelectButtonSprite,
        nullptr,
        this,
        menu_selector(LabelFontCell::Use)
    );
    SelectButton->setPosition({74, 10});
    bMenu->addChild(SelectButton);

    return true;
}

void LabelFontCell::Use(CCObject*){
    //static_cast<LabelSettingsLayer*>(m_SettingsLayer)->FontSelected(m_FontID);
}