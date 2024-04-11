#include "../layers/RunAllowedCell.hpp"
#include "../layers/DTLayer.hpp"

RunAllowedCell* RunAllowedCell::create(int Precent, CCNode* DTLayer) {
    auto ret = new RunAllowedCell();
    if (ret && ret->init(Precent, DTLayer)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RunAllowedCell::init(int Precent, CCNode* DTLayer){

    m_Precent = Precent;
    m_DTLayer = DTLayer;

    auto label = CCLabelBMFont::create((std::to_string(m_Precent) + "%").c_str(), "bigFont.fnt");
    label->setScale(0.35f);
    label->setAnchorPoint({0, 0.5f});
    label->setPosition({2, 10});
    this->addChild(label);

    auto bMenu = CCMenu::create();
    bMenu->setPosition({0,0});
    this->addChild(bMenu);

    auto SelectButtonSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    SelectButtonSprite->setScale(0.35f);
    auto SelectButton = CCMenuItemSpriteExtra::create(
        SelectButtonSprite,
        nullptr,
        this,
        menu_selector(RunAllowedCell::DeleteMe)
    );
    SelectButton->setPosition({62, 10});
    bMenu->addChild(SelectButton);

    return true;
}

void RunAllowedCell::DeleteMe(CCObject*){
    for (int i = 0; i < static_cast<DTLayer*>(m_DTLayer)->m_MyLevelStats.RunsToSave.size(); i++)
    {
        if (static_cast<DTLayer*>(m_DTLayer)->m_MyLevelStats.RunsToSave[i] == m_Precent){
            static_cast<DTLayer*>(m_DTLayer)->m_MyLevelStats.RunsToSave.erase(std::next(static_cast<DTLayer*>(m_DTLayer)->m_MyLevelStats.RunsToSave.begin(), i));
            break;
        }
    }
    static_cast<DTLayer*>(m_DTLayer)->refreshRunAllowedListView();
    static_cast<DTLayer*>(m_DTLayer)->updateRunsAllowed();
}