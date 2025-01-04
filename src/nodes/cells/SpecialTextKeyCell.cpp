#include "../cells/SpecialTextKeyCell.hpp"

SpecialTextKeyCell* SpecialTextKeyCell::create(const std::string& textKey, const std::string& description, const std::function<void(const std::string&)>& onAdd) {
    auto ret = new SpecialTextKeyCell();
    if (ret && ret->init(textKey, description, onAdd)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool SpecialTextKeyCell::init(const std::string& textKey, const std::string& description, const std::function<void(const std::string&)>& onAdd){
    m_TextKey = textKey;
    m_Description = description;
    m_Callback = onAdd;

    auto label = CCLabelBMFont::create(m_TextKey.c_str(), "bigFont.fnt");
    label->setScale(0.35f);
    label->setAnchorPoint({0, 0.5f});
    label->setPosition({2, 10});
    this->addChild(label);

    auto bMenu = CCMenu::create();
    bMenu->setPosition({0,0});
    this->addChild(bMenu);

    auto AddButtonSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    AddButtonSprite->setScale(0.3f);
    auto AddButton = CCMenuItemSpriteExtra::create(
        AddButtonSprite,
        nullptr,
        this,
        menu_selector(SpecialTextKeyCell::Add)
    );
    AddButton->setPosition({65, 10});
    bMenu->addChild(AddButton);

    auto InfoButtonSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    InfoButtonSprite->setScale(0.45f);
    auto InfoButton = CCMenuItemSpriteExtra::create(
        InfoButtonSprite,
        nullptr,
        this,
        menu_selector(SpecialTextKeyCell::onInfo)
    );
    InfoButton->setPosition({50, 10});
    bMenu->addChild(InfoButton);

    return true;
}

void SpecialTextKeyCell::Add(CCObject*){
    if (m_Callback != NULL)
        m_Callback(m_TextKey);
}

void SpecialTextKeyCell::onInfo(CCObject*){
    auto alert = FLAlertLayer::create(m_TextKey.c_str(), m_Description, "Ok");
    alert->show();
}