#include "../cells/LabelFontCell.hpp"

LabelFontCell* LabelFontCell::create(const int& FontID, const std::string& FontFNT, const std::string& FontName, const std::function<void(const int&)>& callback) {
    auto ret = new LabelFontCell();
    if (ret && ret->init(FontID, FontFNT, FontName, callback)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LabelFontCell::init(const int& FontID, const std::string& FontFNT, const std::string& FontName, const std::function<void(const int&)>& callback){
    m_FontID = FontID;
    m_Callback = callback;

    auto label = CCLabelBMFont::create(FontName.c_str(), FontFNT.c_str());
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
    if (m_Callback != NULL)
        m_Callback(m_FontID);
}