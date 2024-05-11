#include "../layers/DTLinkLayer.hpp"
#include "../layers/LinkLevelCell.hpp"
#include "../hooks/dfdsgfsd.h"

DTLinkLayer* DTLinkLayer::create(DTLayer* const& layer) {
    auto ret = new DTLinkLayer();
    if (ret && ret->init(520, 280, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTLinkLayer::setup(DTLayer* const& layer) {
    //create trackerLayer
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setOpacity(0);

    auto overallInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    overallInfoBS->setScale(0.8f);
    auto overallInfoButton = CCMenuItemSpriteExtra::create(
        overallInfoBS,
        nullptr,
        this,
        menu_selector(DTLinkLayer::onOverallInfo)
    );
    overallInfoButton->setPosition(m_size.width / 2 - 8.5f, m_size.height / 2 - 8.5f);
    this->m_buttonMenu->addChild(overallInfoButton);

    alighmentNode = CCNode::create();
    alighmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(alighmentNode);

    m_DTLayer = layer;

    m_AllLevels = StatsManager::getAllLevels();

    refreshLists();

    seartchInput = InputNode::create(225, "Search");
    seartchInput->getInput()->setDelegate(this);
    seartchInput->setPosition({0, 116});
    seartchInput->setScale(0.6f);
    alighmentNode->addChild(seartchInput);

    auto downloadBS = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
    downloadBS->setScale(.8f);
    auto downloadButton = CCMenuItemSpriteExtra::create(
        downloadBS,
        nullptr,
        this,
        menu_selector(DTLinkLayer::onDownload)
    );
    downloadButton->setPosition(-m_size.width / 2 + 3.f, -m_size.height / 2 + 3.f);
    m_buttonMenu->addChild(downloadButton);

    scheduleUpdate();

    return true;
}

void DTLinkLayer::SpacialEditList(GJListLayer* list, CCPoint titlePos, float titleSize){
    CCObject* child;

    CCARRAY_FOREACH(list->m_listView->m_tableView->m_cellArray, child){
        auto childCell = dynamic_cast<GenericListCell*>(child);
        if (childCell)
            childCell->m_backgroundLayer->setOpacity(30);
    }

    std::vector<CCSprite*> spritesToRemove;
    CCLabelBMFont* title;

    CCARRAY_FOREACH(list->getChildren(), child){
        auto childSprite = dynamic_cast<CCSprite*>(child);
        auto childLabel = dynamic_cast<CCLabelBMFont*>(child);
        if (childSprite)
            spritesToRemove.push_back(childSprite);
        if (childLabel)
            title = childLabel;
    }

    for (int i = 0; i < spritesToRemove.size(); i++)
    {
        spritesToRemove[i]->removeMeAndCleanup();
    }

    title->setScale(titleSize);
    if (titlePos.x != -2000)
        title->setPosition({titlePos.x, titlePos.y});
}

void DTLinkLayer::refreshLists(){
    if (m_LevelsList != nullptr) m_LevelsList->removeMeAndCleanup();
    if (m_LinkedLevelsList != nullptr) m_LinkedLevelsList->removeMeAndCleanup();

    auto levelsListItems = CCArray::create();

    auto linkedLevelsListItems = CCArray::create();

    auto myKey = StatsManager::getLevelKey(m_DTLayer->m_Level);

    for (int i = 0; i < m_AllLevels.size(); i++)
    {
        std::string filterTextLower = "";
        for (char ch : m_filterText) { 
            filterTextLower += std::tolower(ch); 
        }

        std::string levelNameLower = "";
        for (char ch : m_AllLevels[i].second.levelName) { 
            levelNameLower += std::tolower(ch); 
        }
        if (m_AllLevels[i].second.levelName == "-1")
            levelNameLower = "unknown name";

        if (m_AllLevels[i].first != myKey && StatsManager::ContainsAtIndex(0, m_filterText, levelNameLower)){
            bool isValidForList = true;
            for (int s = 0; s < m_DTLayer->m_MyLevelStats.LinkedLevels.size(); s++)
            {
                if (m_DTLayer->m_MyLevelStats.LinkedLevels[s] == m_AllLevels[i].first)
                    isValidForList = false;
            }
            
            if (isValidForList){
                levelsListItems->addObject(LinkLevelCell::create(this, m_AllLevels[i].first, m_AllLevels[i].second, false));
            }
            else{
                linkedLevelsListItems->addObject(LinkLevelCell::create(this, m_AllLevels[i].first, m_AllLevels[i].second, true));
            }
        }
    }

    auto levelsListView = ListView::create(levelsListItems, 40, CellsWidth, 220);

    m_LevelsList = GJListLayer::create(levelsListView, "Levels", {0,0,0,75}, CellsWidth, 220, 1);
    m_LevelsList->setPosition({-233, -116});
    alighmentNode->addChild(m_LevelsList);

    SpacialEditList(m_LevelsList, {m_LevelsList->getContentSize().width / 2, 234}, 0.7f);

    auto linkedLevelsListView = ListView::create(linkedLevelsListItems, 40, CellsWidth, 220);

    m_LinkedLevelsList = GJListLayer::create(linkedLevelsListView, "Linked", {0,0,0,75}, CellsWidth, 220, 1);
    m_LinkedLevelsList->setPosition({2, -116});
    alighmentNode->addChild(m_LinkedLevelsList);

    SpacialEditList(m_LinkedLevelsList, {m_LinkedLevelsList->getContentSize().width / 2, 234}, 0.7f);
}

void DTLinkLayer::ChangeLevelLinked(std::string levelKey, LevelStats stats, bool erase){
    if (erase){
        for (int i = 0; i < m_DTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
        {
            if (m_DTLayer->m_MyLevelStats.LinkedLevels[i] == levelKey)
            {
                m_DTLayer->m_MyLevelStats.LinkedLevels.erase(std::next(m_DTLayer->m_MyLevelStats.LinkedLevels.begin(), i));
                break;
            }
        }

        for (int i = 0; i < stats.LinkedLevels.size(); i++)
        {
            if (stats.LinkedLevels[i] == StatsManager::getLevelKey(m_DTLayer->m_Level))
            {
                stats.LinkedLevels.erase(std::next(stats.LinkedLevels.begin(), i));
                break;
            }
        }
    }
    else{
        m_DTLayer->m_MyLevelStats.LinkedLevels.push_back(levelKey);

        stats.LinkedLevels.push_back(StatsManager::getLevelKey(m_DTLayer->m_Level));
    }

    refreshLists();

    if (!erase){
        m_LevelsList->retain();
        m_LevelsList->removeFromParent();
        alighmentNode->addChild(m_LevelsList);
        m_LevelsList->release();
    }
    else{
        m_LinkedLevelsList->retain();
        m_LinkedLevelsList->removeFromParent();
        alighmentNode->addChild(m_LinkedLevelsList);
        m_LinkedLevelsList->release();
    }

    StatsManager::saveData(m_DTLayer->m_MyLevelStats, m_DTLayer->m_Level);

    StatsManager::saveData(stats, levelKey);
}

void DTLinkLayer::update(float delta){

    CCRect LevelsListRect = {m_LevelsList->getPositionX(), m_LevelsList->getPositionY(), m_LevelsList->getScaledContentSize().width, m_LevelsList->getScaledContentSize().height};

    CCRect LinkedLevelsListRect = {m_LinkedLevelsList->getPositionX(), m_LinkedLevelsList->getPositionY(), m_LinkedLevelsList->getScaledContentSize().width, m_LinkedLevelsList->getScaledContentSize().height};

    if (LevelsListRect.containsPoint(convertToNodeSpace(getMousePos())) && !scrollSwitchLock){
        scrollSwitchLock = true;

        m_LevelsList->retain();
        m_LevelsList->removeFromParent();
        alighmentNode->addChild(m_LevelsList);
        m_LevelsList->release();
    }

    if (LinkedLevelsListRect.containsPoint(convertToNodeSpace(getMousePos())) && scrollSwitchLock){
        scrollSwitchLock = false;

        m_LinkedLevelsList->retain();
        m_LinkedLevelsList->removeFromParent();
        alighmentNode->addChild(m_LinkedLevelsList);
        m_LinkedLevelsList->release();
    }
    if (downloadingInfo){
        if (loadingProgress == 0 && m_LoadLevelsCircle->isVisible()){
            loadingProgress = 1;
        }
        if (loadingProgress == 1 && !m_LoadLevelsCircle->isVisible()){
            loadingProgress = 2;
        }
        else if (loadingProgress == 2){
            loadingProgress = 0;
            CCObject* child;

            CCARRAY_FOREACH(m_LoadLevelsBypass->m_list->m_listView->m_tableView->m_cellArray, child){
                auto level = static_cast<LevelCell*>(child)->m_level;
                if (ghc::filesystem::exists(StatsManager::getLevelSaveFilePath(level))){
                    auto stats = StatsManager::getLevelStats(level);

                    stats.attempts = level->m_attempts;
                    stats.levelName = level->m_levelName;
                    stats.difficulty = StatsManager::getDifficulty(level);

                    StatsManager::saveData(stats, level);
                }
            }

            refreshIfDownloadDone();
            m_LoadLevelsBypass->removeMeAndCleanup();
            m_LoadLevelsBypass = nullptr;
        }
    }
}

void DTLinkLayer::textChanged(CCTextInputNode* input){
    if (seartchInput->getInput() == input){
        std::string filterText = "";
        if (input->getString() != "")
            filterText = input->getString();
        
        m_filterText = filterText;
        refreshLists();
    }
}

void DTLinkLayer::onClose(CCObject*) {
    m_DTLayer->UpdateSharedStats();
    m_DTLayer->refreshStrings();
    m_DTLayer->RefreshText();
    m_DTLayer->refreshRunAllowedListView();
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void DTLinkLayer::onDownload(CCObject*){
    if (downloadingInfo) return;

    std::vector<int> levelIDs;

    for (int i = 0; i < m_AllLevels.size(); i++)
    {
        auto splittedLevelKey = StatsManager::splitLevelKey(m_AllLevels[i].first);
        if (splittedLevelKey.second == "online"){
            int id = std::stoi(splittedLevelKey.first);

            levelIDs.push_back(id);
        }
    }

    auto list = GJLevelList::create();
	list->m_listName = "t";
	list->m_levels = levelIDs;

    m_LoadLevelsBypass = dfdsgfsd::create(list);
    this->addChild(m_LoadLevelsBypass);

    CCObject* child;

    CCARRAY_FOREACH(m_LoadLevelsBypass->getChildren(), child){
        auto loadingC = dynamic_cast<LoadingCircle*>(child);
        if (!loadingC)
            static_cast<CCNode*>(child)->setVisible(false);
        else
            m_LoadLevelsCircle = loadingC;
    }

    downloadingInfo = true;
}

void DTLinkLayer::refreshIfDownloadDone(){
    if (downloadingInfo){
        downloadingInfo = false;

        m_AllLevels = StatsManager::getAllLevels();
        refreshLists();
    }
}

void DTLinkLayer::onOverallInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "On the left you have a list of all levels you have <cy>saved progress</c> on, on the right is the list of levels that are <cg>linked</c> to this one :)\nClick the arrows to move them around and use the search bar if you want.\n \nLinked levels will share stats.", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}
