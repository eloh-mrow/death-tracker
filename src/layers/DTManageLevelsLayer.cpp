#include "../layers/DTManageLevelsLayer.hpp"
#include "../layers/LinkLevelCell.hpp"

DTManageLevelsLayer* DTManageLevelsLayer::create(DTLayer* const& layer) {
    auto ret = new DTManageLevelsLayer();
    if (ret && ret->init(380, 280, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTManageLevelsLayer::setup(DTLayer* const& layer) {
    //create trackerLayer
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_DTLayer = layer;

    m_AllLevels = StatsManager::getAllLevels();

    seartchInput = InputNode::create(225, "Search");
    seartchInput->getInput()->setDelegate(this);
    seartchInput->setPosition({winSize.width / 2, 277});
    seartchInput->setScale(0.6f);
    this->addChild(seartchInput);

    refreshLists(false);

    return true;
}

void DTManageLevelsLayer::textChanged(CCTextInputNode* input){
    if (seartchInput->getInput() == input){
        std::string filterText = "";
        if (input->getString() != "")
            filterText = input->getString();
        
        m_filterText = filterText;
        refreshLists(false);
    }
}

void DTManageLevelsLayer::refreshLists(bool SavePos){
    int prevY = 1;
    if (m_LevelsList){
        prevY = m_LevelsList->m_listView->m_tableView->m_contentLayer->getPositionY();
        m_LevelsList->removeMeAndCleanup();
    }

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto myKey = StatsManager::getLevelKey(m_DTLayer->m_Level);
    auto levelsListItems = CCArray::create();

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
            levelsListItems->addObject(LinkLevelCell::create(this, m_AllLevels[i].first, m_AllLevels[i].second, false));
        }
    }

    auto levelsListView = ListView::create(levelsListItems, 40, CellsWidth, 220);

    m_LevelsList = GJListLayer::create(levelsListView, "", {0,0,0,75}, CellsWidth, 220, 1);
    m_LevelsList->setPosition({130, 42});
    if (prevY != 1 && SavePos){
        m_LevelsList->m_listView->m_tableView->m_contentLayer->setPositionY(prevY);
    }
    this->addChild(m_LevelsList);

    SpacialEditList(m_LevelsList, {m_LevelsList->getContentSize().width / 2, 234}, 0.7f);
}

void DTManageLevelsLayer::SpacialEditList(GJListLayer* list, CCPoint titlePos, float titleSize){
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