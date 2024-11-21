#include "../layers/ChooseRunCell.hpp"

ChooseRunCell* ChooseRunCell::create(const int& Percent, const std::function<void(const int&)>& callback) {
    auto ret = new ChooseRunCell();
    if (ret && ret->init(Percent, callback)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool ChooseRunCell::init(const int& Percent, const std::function<void(const int&)>& callback){
    m_Percent = Percent;
    m_Callback = callback;

    auto label = CCLabelBMFont::create(fmt::format("{}%", m_Percent).c_str(), "bigFont.fnt");
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
        menu_selector(ChooseRunCell::ChooseMe)
    );
    SelectButton->setPosition({54, 10});
    bMenu->addChild(SelectButton);

    return true;
}


void ChooseRunCell::ChooseMe(CCObject*){
    if (m_Callback != NULL)
        m_Callback(m_Percent);
}