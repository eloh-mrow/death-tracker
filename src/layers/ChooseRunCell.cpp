#include "../layers/ChooseRunCell.hpp"
#include "../layers/DTGraphLayer.hpp"

ChooseRunCell* ChooseRunCell::create(int Precent, CCNode* DTGLayer) {
    auto ret = new ChooseRunCell();
    if (ret && ret->init(Precent, DTGLayer)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool ChooseRunCell::init(int Precent, CCNode* DTGLayer){

    m_Precent = Precent;
    m_DTGraphLayer = DTGLayer;

    auto label = CCLabelBMFont::create((std::to_string(m_Precent) + "%").c_str(), "bigFont.fnt");
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
    static_cast<DTGraphLayer*>(m_DTGraphLayer)->RunChosen(m_Precent);
}