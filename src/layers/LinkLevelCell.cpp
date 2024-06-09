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

LinkLevelCell* LinkLevelCell::create(DTManageLevelsLayer* DTMLL, std::string levelKey, LevelStats stats, bool linked) {
    auto ret = new LinkLevelCell();
    if (ret && ret->init(DTMLL, levelKey, stats, linked)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LinkLevelCell::init(CCNode* l, std::string levelKey, LevelStats stats, bool linked){

    m_LevelKey = levelKey;
    m_Stats = stats;
    m_Linked = linked;

    float cellW = 0;

    if (dynamic_cast<DTLinkLayer*>(l)){
        m_DTLinkLayer = dynamic_cast<DTLinkLayer*>(l);
        cellW = m_DTLinkLayer->CellsWidth;
    }
    else
    {
        m_DTManageLevelsLayer = dynamic_cast<DTManageLevelsLayer*>(l);
        cellW = m_DTManageLevelsLayer->CellsWidth;
    }
    
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

    if (m_DTLinkLayer){
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
    }
    else{
        auto deleteLevelBS = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        deleteLevelBS->setScale(0.6f);
        auto deleteLevelButton = CCMenuItemSpriteExtra::create(
            deleteLevelBS,
            nullptr,
            this,
            menu_selector(LinkLevelCell::DeleteMe)
        );
        deleteLevelButton->setPosition({282, 20});
        bMenu->addChild(deleteLevelButton);

        auto viewBS = ButtonSprite::create("View");
        viewBS->setScale(0.6f);
        auto viewButton = CCMenuItemSpriteExtra::create(
            viewBS,
            nullptr,
            this,
            menu_selector(LinkLevelCell::ViewMe)
        );
        viewButton->setPosition({242, 20});
        bMenu->addChild(viewButton);
    }

    scheduleUpdate();

    return true;
}

void LinkLevelCell::MoveMe(CCObject*){
    m_DTLinkLayer->ChangeLevelLinked(m_LevelKey, m_Stats, m_Linked);
}

void LinkLevelCell::DeleteMe(CCObject*){
    DeleteWarningAlert = FLAlertLayer::create(this, "Warning!", fmt::format("Doing this will delete all progress saved on <cy>{}</c>\n \nAre you sure you want to delete it?", m_Stats.levelName), "No", "Yes");
    DeleteWarningAlert->setZOrder(105);
    m_DTManageLevelsLayer->addChild(DeleteWarningAlert);
}

void LinkLevelCell::ViewMe(CCObject*){
    if (m_DTManageLevelsLayer->dInfo) return;

    m_DTManageLevelsLayer->downloadCircle->setVisible(true);

    GameLevelManager::get()->m_levelManagerDelegate = this;
    GameLevelManager::get()->getOnlineLevels(GJSearchObject::create(SearchType::Search, StatsManager::splitLevelKey(m_LevelKey).first));
    
    downloadingInfo = true;
    m_DTManageLevelsLayer->dInfo = true;
}

void LinkLevelCell::loadLevelsFinished(cocos2d::CCArray* levels, char const* hash){
    downloadingInfo = false;
    m_DTManageLevelsLayer->dInfo = false;

    m_DTManageLevelsLayer->downloadCircle->setVisible(false);

    if (levels->count() == 0) {
        log::info("failed to fetch level");
    }

    auto* level = static_cast<GJGameLevel*>(
        levels->objectAtIndex(0)
    );

    auto layer = LevelInfoLayer::create(level, false);
    auto scene = CCScene::create();

    scene->addChild(layer);
    auto transition = CCTransitionFade::create(0.5f, scene);

    CCDirector::sharedDirector()->pushScene(transition);
}

void LinkLevelCell::loadLevelsFinished(cocos2d::CCArray* p0, char const* p1, int p2){
    loadLevelsFinished(p0, p1);
}

void LinkLevelCell::loadLevelsFailed(char const* p0){
    downloadingInfo = false;
    m_DTManageLevelsLayer->dInfo = false;
    m_DTManageLevelsLayer->downloadCircle->setVisible(false);
    log::info("failed to fetch level");
}

void LinkLevelCell::loadLevelsFailed(char const* p0, int p1){
    loadLevelsFailed(p0);
}

void LinkLevelCell::FLAlert_Clicked(FLAlertLayer* alert, bool selected){
    if (DeleteWarningAlert == alert && selected){
        if (m_DTManageLevelsLayer->dInfo) return;

        for (int i = 0; i < m_DTManageLevelsLayer->m_AllLevels.size(); i++)
        {
            if (m_DTManageLevelsLayer->m_AllLevels[i].first == m_LevelKey){
                m_DTManageLevelsLayer->m_AllLevels.erase(std::next(m_DTManageLevelsLayer->m_AllLevels.begin(), i));
                break;
            }
        }
        
        std::filesystem::path filep = Mod::get()->getSaveDir() / "levels" / (m_LevelKey + ".json");

        std::filesystem::remove(filep);

        m_DTManageLevelsLayer->refreshLists(true);
    }
    
}
