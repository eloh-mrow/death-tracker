#include "../layers/LinkLevelCell.hpp"

LinkLevelCell* LinkLevelCell::create(DTLinkLayer* DTL, std::string levelKey, LevelStats stats, bool linked) {
    auto ret = new LinkLevelCell();
    if (ret && ret->init(DTL, levelKey, stats, linked)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LinkLevelCell::init(DTLinkLayer* DTL, std::string levelKey, LevelStats stats, bool linked){

    m_LevelKey = levelKey;
    m_Stats = stats;
    m_Linked = linked;
    m_DTLinkLayer = DTL;

    auto splittedKey = StatsManager::splitLevelKey(levelKey);

    std::string nameToDisplay = stats.levelName;
    if (nameToDisplay == "-1"){
        nameToDisplay = "Unknow Name";
    }

    CCSprite* difficultySprite;
    if (stats.difficulty == -1){
        difficultySprite = CCSprite::createWithSpriteFrameName("diffIcon_auto_btn_001.png");
    }
    else{
        if (stats.difficulty < 10){
            difficultySprite = CCSprite::createWithSpriteFrameName(fmt::format("diffIcon_0{}_btn_001.png", stats.difficulty).c_str());
        }
        else{
            difficultySprite = CCSprite::createWithSpriteFrameName("diffIcon_10_btn_001.png");
        }
    }
    difficultySprite->setPosition({17, 20});
    if (linked){
        difficultySprite->setPosition({DTL->CellsWidth - 17, 20});
    }
    difficultySprite->setScale(0.7f);
    this->addChild(difficultySprite);

    std::string attemptsToDisplay = std::to_string(stats.attempts);
    if (attemptsToDisplay == "-1"){
        attemptsToDisplay = "Unknow Attempts";
    }

    auto nameLabel = CCLabelBMFont::create(nameToDisplay.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.5f);
    nameLabel->setAnchorPoint({0, 0.5f});
    nameLabel->setPosition({32, 28});
    if (linked){
        nameLabel->setPosition({DTL->CellsWidth - 32, 28});
        nameLabel->setAnchorPoint({1, 0.5f});
        nameLabel->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
    }
    this->addChild(nameLabel);

    auto extraLabel = CCLabelBMFont::create(fmt::format("ID: {} type: {}\nattempt: {}", splittedKey.first, splittedKey.second, attemptsToDisplay).c_str(), "bigFont.fnt");
    extraLabel->setScale(0.25f);
    extraLabel->setAnchorPoint({0, 0.5f});
    extraLabel->setPosition({32, 14});
    if (linked){
        extraLabel->setPosition({DTL->CellsWidth - 32, 14});
        extraLabel->setAnchorPoint({1, 0.5f});
        extraLabel->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
    }
    this->addChild(extraLabel);

    auto bMenu = CCMenu::create();
    bMenu->setPosition({0,0});
    this->addChild(bMenu);

    CCSprite* MoveButtonSprite;
    if (linked){
        MoveButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        MoveButtonSprite->setScale(0.5f);
    }
    else{
        MoveButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        MoveButtonSprite->setScaleX(-0.5f);
        MoveButtonSprite->setScaleY(0.5f);
    }
    auto MoveButton = CCMenuItemSpriteExtra::create(
        MoveButtonSprite,
        nullptr,
        this,
        menu_selector(LinkLevelCell::MoveMe)
    );

    bMenu->addChild(MoveButton);

    if (linked){
        MoveButton->setPosition({15, 20});
    }
    else{
        MoveButton->setPosition({DTL->CellsWidth - 15, 20});
        MoveButton->setContentSize({-MoveButton->getContentSize().width, MoveButton->getContentSize().height});
        MoveButtonSprite->setPositionX(-MoveButtonSprite->getPositionX());
    }

    scheduleUpdate();

    return true;
}

void LinkLevelCell::MoveMe(CCObject*){
    m_DTLinkLayer->ChangeLevelLinked(m_LevelKey, m_Stats, m_Linked);
}
