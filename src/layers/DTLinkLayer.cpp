#include "../layers/DTLinkLayer.hpp"
#include "../layers/LinkLevelCell.hpp"

DTLinkLayer* DTLinkLayer::create(DTLayer* const& layer) {
    auto ret = new DTLinkLayer();
    if (ret && ret->initAnchored(540, 280, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTLinkLayer::setup(DTLayer* const& layer) {
    //create trackerLayer
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto overallInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    overallInfoBS->setScale(0.8f);
    auto overallInfoButton = CCMenuItemSpriteExtra::create(
        overallInfoBS,
        nullptr,
        this,
        menu_selector(DTLinkLayer::onOverallInfo)
    );
    overallInfoButton->setPosition(m_size.width - 8.5f, m_size.height - 8.5f);
    this->m_buttonMenu->addChild(overallInfoButton);

    alignmentNode = CCNode::create();
    alignmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(alignmentNode);

    m_DTLayer = layer;

    auto levelsMoveRightS = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    levelsMoveRightS->setScale(0.5f);
    levelsMoveRightS->setRotation(180);
    levelsMoveRight = CCMenuItemSpriteExtra::create(
        levelsMoveRightS,
        nullptr,
        this,
        menu_selector(DTLinkLayer::levelsListMoveRight)
    );
    levelsMoveRight->setPosition({this->m_buttonMenu->getContentWidth() / 2 - 2, this->m_buttonMenu->getContentHeight() / 2});
    levelsMoveRight->setVisible(false);
    this->m_buttonMenu->addChild(levelsMoveRight);

    auto levelsMoveLeftS = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    levelsMoveLeftS->setScale(0.5f);
    levelsMoveLeft = CCMenuItemSpriteExtra::create(
        levelsMoveLeftS,
        nullptr,
        this,
        menu_selector(DTLinkLayer::levelsListMoveLeft)
    );
    levelsMoveLeft->setPosition({this->m_buttonMenu->getContentWidth() / 2 -252, this->m_buttonMenu->getContentHeight() / 2});
    levelsMoveLeft->setVisible(false);
    this->m_buttonMenu->addChild(levelsMoveLeft);

    seartchInput = TextInput::create(225, "Search");
    seartchInput->setPosition({0, 116});
    seartchInput->setScale(0.6f);
    alignmentNode->addChild(seartchInput);

    loading = LoadingCircle::create();
    loading->setParentLayer(this);
    loading->show();

    std::thread levelsThread = std::thread([this](){
        auto AllLevelsRes = StatsManager::getAllLevels();

        if (AllLevelsRes.isOk()){
            auto tempAllLevels = AllLevelsRes.unwrap();

            geode::Loader::get()->queueInMainThread([tempAllLevels, this](){
                onLevelsLoaded(tempAllLevels);
            });
        }
        else{
            geode::Notification::create("Data save path invalid!", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
        }
    });
    levelsThread.detach();

    return true;
}

void DTLinkLayer::onLevelsLoaded(const std::vector<std::pair<std::string, LevelStats>>& levels){
    if (isClosed) return;
    m_AllLevels = levels;
    this->refreshLists();
    seartchInput->getInputNode()->setDelegate(this);
    this->scheduleUpdate();
    levelsMoveLeft->setVisible(true);
    levelsMoveRight->setVisible(true);
    loading->setVisible(false);
}

void DTLinkLayer::SpacialEditList(GJListLayer* const& list, const CCPoint& titlePos, const float& titleSize){
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
    if (!m_AllLevels.size()) return;

    auto levelsListItems = CCArray::create();

    auto linkedLevelsListItems = CCArray::create();

    auto myKey = StatsManager::getLevelKey(m_DTLayer->m_Level).unwrapOr("-1");
    if (myKey == "-1")
        return;

    std::vector<std::pair<std::string, LevelStats>> AllLevelsSearch{};

    for (int i = 0; i < m_AllLevels.size(); i++)
    {
        std::string levelNameLower;
        for (char ch : m_AllLevels[i].second.levelName) { 
            levelNameLower += std::tolower(ch);
        }
        if (m_AllLevels[i].second.levelName == "-1")
            levelNameLower = "unknown name";
        
        if (m_AllLevels[i].first != myKey && StatsManager::isKeyInIndex(levelNameLower, 0, m_filterText))
            AllLevelsSearch.push_back(m_AllLevels[i]);
    }

    if (!AllLevelsSearch.size() && m_LevelsList && m_LinkedLevelsList){
        m_LevelsList->m_listView->m_tableView->m_contentLayer->removeAllChildrenWithCleanup(true);
        m_LinkedLevelsList->m_listView->m_tableView->m_contentLayer->removeAllChildrenWithCleanup(true);
        return;
    }

    if (m_LevelsList != nullptr) m_LevelsList->removeMeAndCleanup();
    if (m_LinkedLevelsList != nullptr) m_LinkedLevelsList->removeMeAndCleanup();

    int startI = (levelPage - 1) * 10;
    if (startI <= 0){
        levelsMoveLeft->setVisible(false);
    }
    else
        levelsMoveLeft->setVisible(true);

    int endI = levelPage * 10;
    if (endI > AllLevelsSearch.size() - 1){
        endI = AllLevelsSearch.size();
        levelsMoveRight->setVisible(false);
    }
    else
        levelsMoveRight->setVisible(true);

    if (AllLevelsSearch.size() <= 0) return;

    std::vector<std::pair<std::string, LevelStats>> allLevelsInRange(std::next(AllLevelsSearch.begin(), startI), std::next(AllLevelsSearch.begin(), endI));
    
    
    std::vector<std::pair<std::string, LevelStats>> linkedOnes{};

    for (int i = 0; i < AllLevelsSearch.size(); i++)
    {
        for (int s = 0; s < m_DTLayer->m_MyLevelStats.LinkedLevels.size(); s++)
        {
            if (m_DTLayer->m_MyLevelStats.LinkedLevels[s] == AllLevelsSearch[i].first)
                linkedLevelsListItems->addObject(LinkLevelCell::create(CellsWidth, AllLevelsSearch[i].first, AllLevelsSearch[i].second, true, std::bind(&DTLinkLayer::ChangeLevelLinked, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
        }
    }
    

    for (int i = 0; i < allLevelsInRange.size(); i++)
    {
        std::string filterTextLower;
        for (char ch : m_filterText) { 
            filterTextLower += std::tolower(ch); 
        }

        bool isValidForList = true;
        for (int s = 0; s < m_DTLayer->m_MyLevelStats.LinkedLevels.size(); s++)
        {
            if (m_DTLayer->m_MyLevelStats.LinkedLevels[s] == allLevelsInRange[i].first)
                isValidForList = false;
        }
            
        if (isValidForList){
            levelsListItems->addObject(LinkLevelCell::create(CellsWidth, allLevelsInRange[i].first, allLevelsInRange[i].second, false, std::bind(&DTLinkLayer::ChangeLevelLinked, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
        }
    }

    auto levelsListView = ListView::create(levelsListItems, 40, CellsWidth, 220);

    m_LevelsList = GJListLayer::create(levelsListView, "Levels", {0,0,0,75}, CellsWidth, 220, 1);
    m_LevelsList->setPosition({-242, -116});
    alignmentNode->addChild(m_LevelsList);

    SpacialEditList(m_LevelsList, {m_LevelsList->getContentSize().width / 2, 234}, 0.7f);

    auto linkedLevelsListView = ListView::create(linkedLevelsListItems, 40, CellsWidth, 220);

    m_LinkedLevelsList = GJListLayer::create(linkedLevelsListView, "Linked", {0,0,0,75}, CellsWidth, 220, 1);
    m_LinkedLevelsList->setPosition({11, -116});
    alignmentNode->addChild(m_LinkedLevelsList);

    SpacialEditList(m_LinkedLevelsList, {m_LinkedLevelsList->getContentSize().width / 2, 234}, 0.7f);
}

void DTLinkLayer::ChangeLevelLinked(const std::string levelKey, LevelStats stats, const bool& erase){
    auto levelStats = StatsManager::getLevelKey(m_DTLayer->m_Level).unwrapOr("-1");
    if (levelStats == "-1"){
        Notification::create("failed to move linked level!", nullptr)->show();
        return;
    }

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
            if (stats.LinkedLevels[i] == levelStats)
            {
                stats.LinkedLevels.erase(std::next(stats.LinkedLevels.begin(), i));
                break;
            }
        }
    }
    else{
        m_DTLayer->m_MyLevelStats.LinkedLevels.push_back(levelKey);

        stats.LinkedLevels.push_back(levelStats);
    }

    refreshLists();

    if (!erase){
        m_LevelsList->retain();
        m_LevelsList->removeFromParent();
        alignmentNode->addChild(m_LevelsList);
        m_LevelsList->release();
    }
    else{
        m_LinkedLevelsList->retain();
        m_LinkedLevelsList->removeFromParent();
        alignmentNode->addChild(m_LinkedLevelsList);
        m_LinkedLevelsList->release();
    }

    if (m_DTLayer->m_MyLevelStats.currentBest != -1)
        StatsManager::saveData(m_DTLayer->m_MyLevelStats, m_DTLayer->m_Level);
    if (stats.currentBest != -1)
        StatsManager::saveData(stats, levelKey);
}

void DTLinkLayer::update(float delta){

    if (m_LevelsList == nullptr || m_LinkedLevelsList == nullptr) return;

    CCRect LevelsListRect = {m_LevelsList->getPositionX(), m_LevelsList->getPositionY(), m_LevelsList->getScaledContentSize().width, m_LevelsList->getScaledContentSize().height};

    CCRect LinkedLevelsListRect = {m_LinkedLevelsList->getPositionX(), m_LinkedLevelsList->getPositionY(), m_LinkedLevelsList->getScaledContentSize().width, m_LinkedLevelsList->getScaledContentSize().height};

    if (LevelsListRect.containsPoint(convertToNodeSpace(getMousePos())) && !scrollSwitchLock){
        scrollSwitchLock = true;

        m_LevelsList->retain();
        m_LevelsList->removeFromParent();
        alignmentNode->addChild(m_LevelsList);
        m_LevelsList->release();
    }

    if (LinkedLevelsListRect.containsPoint(convertToNodeSpace(getMousePos())) && scrollSwitchLock){
        scrollSwitchLock = false;

        m_LinkedLevelsList->retain();
        m_LinkedLevelsList->removeFromParent();
        alignmentNode->addChild(m_LinkedLevelsList);
        m_LinkedLevelsList->release();
    }
}

void DTLinkLayer::textChanged(CCTextInputNode* input){
    levelPage = 1;
    if (seartchInput->getInputNode() == input){
        std::string filterText;
        if (!input->getString().empty())
            filterText = input->getString();
        
        m_filterText = filterText;
        refreshLists();
    }
}

void DTLinkLayer::onClose(CCObject*) {
    isClosed = true;
    m_DTLayer->UpdateSharedStats();
    m_DTLayer->refreshAll(true);
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void DTLinkLayer::onOverallInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "On the left you have a list of all levels you have <cy>saved progress</c> on, on the right is the list of levels that are <cg>linked</c> to this one :)\nClick the arrows to move them around and use the search bar if you want.\n \nLinked levels will share stats.", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}

void DTLinkLayer::levelsListMoveLeft(CCObject*){
    levelPage--;
    refreshLists();
}
void DTLinkLayer::levelsListMoveRight(CCObject*){
    levelPage++;
    refreshLists();
}