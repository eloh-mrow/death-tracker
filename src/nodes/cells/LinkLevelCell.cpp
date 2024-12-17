#include "../cells/LinkLevelCell.hpp"
#include "../../utils/Settings.hpp"

LinkLevelCell* LinkLevelCell::create(const float& cellW, const std::string& levelKey, const LevelStats& stats, const bool& linked, const std::function<void(const std::string, LevelStats , const bool &)>& callback) {
    auto ret = new LinkLevelCell();
    if (ret && ret->init(cellW, levelKey, stats, linked, callback)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LinkLevelCell::init(const float& cellW, const std::string& levelKey, const LevelStats& stats, const bool& linked, const std::function<void(const std::string, LevelStats, const bool &)>& callback){

    m_LevelKey = levelKey;
    m_Stats = stats;
    m_Linked = linked;

    m_Callback = callback;
    
    auto splittedKey = StatsManager::splitLevelKey(levelKey);

    std::string nameToDisplay = stats.levelName;
    if (nameToDisplay == "-1"){
        nameToDisplay = "Unknown Name";
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
        difficultySprite->setPosition({cellW - 17, 20});
    }
    difficultySprite->setScale(0.7f);
    this->addChild(difficultySprite);

    std::string attemptsToDisplay = std::to_string(stats.attempts);
    if (attemptsToDisplay == "-1"){
        attemptsToDisplay = "Unknown Attempts";
    }

    auto nameLabel = CCLabelBMFont::create(nameToDisplay.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.5f);
    nameLabel->setAnchorPoint({0, 0.5f});
    nameLabel->setPosition({32, 28});
    if (linked){
        nameLabel->setPosition({cellW - 32, 28});
        nameLabel->setAnchorPoint({1, 0.5f});
        nameLabel->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
    }
    this->addChild(nameLabel);

    auto extraLabel = CCLabelBMFont::create(fmt::format("ID: {} type: {}\nattempts: {}", splittedKey.first, splittedKey.second, attemptsToDisplay).c_str(), "bigFont.fnt");
    extraLabel->setScale(0.25f);
    extraLabel->setAnchorPoint({0, 0.5f});
    extraLabel->setPosition({32, 14});
    if (linked){
        extraLabel->setPosition({cellW - 32, 14});
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
        MoveButton->setPosition({cellW - 15, 20});
        MoveButton->setContentSize({-MoveButton->getContentSize().width, MoveButton->getContentSize().height});
        MoveButtonSprite->setPositionX(-MoveButtonSprite->getPositionX());
    }

    scheduleUpdate();

    return true;
}

void LinkLevelCell::MoveMe(CCObject*){
    if (m_Callback != NULL)
        m_Callback(m_LevelKey, m_Stats, m_Linked);
}