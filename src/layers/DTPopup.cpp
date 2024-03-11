#include "../layers/DTPopup.hpp"
#include "../managers/StatsManager.hpp"

DTPopup* DTPopup::create(float width, float hight, FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level) {
    auto ret = new DTPopup();
    if (ret && ret->init(width, hight, InfoAlertLayer, Level, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTPopup::setup(FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level) {

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_InfoAlertLayer = InfoAlertLayer;
    m_Level = Level;

    this->setTitle("Deaths");

    m_buttonMenu->setPosition({0, 0});

    m_closeBtn->removeMeAndCleanup();
    auto closeButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    m_closeBtn = CCMenuItemSpriteExtra::create(
        closeButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::onClose)
    );
    m_buttonMenu->addChild(m_closeBtn);

    m_closeBtn->setPosition({winSize.width / 23.708f, winSize.height / 1.079f});

    auto SLayer = ScrollLayer::create({m_size.width / 1.25f, m_size.height / 1.45f});
    SLayer->setZOrder(1);
    SLayer->setPosition(winSize / 2 - SLayer->getContentSize() / 2);
    this->m_mainLayer->addChild(SLayer);

    auto ScrollLayerBG = CCScale9Sprite::create("square02_small.png");
    ScrollLayerBG->setContentSize(SLayer->getContentSize());
    ScrollLayerBG->setOpacity(105);
    ScrollLayerBG->setPosition(winSize / 2);
    this->m_mainLayer->addChild(ScrollLayerBG);

    auto deathStrings = CreateDeathsStrings();

    log::info("hi");
    if (deathStrings.first != "-1"){
        log::info("found file");
        if (deathStrings.first == "No Saved Progress"){
            log::info("didnt find progress");
        }
        else{
            auto DeathsLabel = SimpleTextArea::create(deathStrings.first, "chatFont.fnt", 0.75f);
            m_TextAreas.push_back(DeathsLabel);
            DeathsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
            DeathsLabel->setAnchorPoint({0.5f, 1});
            DeathsLabel->setPosition({SLayer->getContentSize().width / 2, SLayer->getContentSize().height});
            SLayer->m_contentLayer->addChild(DeathsLabel);

            auto RunsLabel = SimpleTextArea::create(deathStrings.second, "chatFont.fnt", 0.75f);
            m_TextAreas.push_back(RunsLabel);
            RunsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
            RunsLabel->setAnchorPoint({0.5f, 1});
            RunsLabel->setPosition({SLayer->getContentSize().width / 2, SLayer->getContentSize().height - DeathsLabel->getContentSize().height});
            SLayer->m_contentLayer->addChild(RunsLabel);

            if (DeathsLabel->getContentSize().height + RunsLabel->getContentSize().height > SLayer->getContentSize().height){
                SLayer->m_contentLayer->setContentSize({SLayer->m_contentLayer->getContentSize().width, DeathsLabel->getContentSize().height + RunsLabel->getContentSize().height});
                for (int i = 0; i < m_TextAreas.size(); i++)
                {
                    m_TextAreas[i]->setPositionY(m_TextAreas[i]->getPositionY() + (SLayer->m_contentLayer->getContentSize().height - SLayer->getContentSize().height));
                }
                
            }
                
            SLayer->moveToTop();
        }
    }

    scheduleUpdate();

    return true;
}

void DTPopup::update(float delta){
    for (int i = 0; i < m_TextAreas.size(); i++)
    {
        m_TextAreas[i]->setVisible(true);
    }
    
}

void DTPopup::onClose(cocos2d::CCObject* object){
    m_InfoAlertLayer->m_mainLayer->setVisible(true);
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

std::pair<std::string, std::string> DTPopup::CreateDeathsStrings(){
    if (!m_Level) return std::pair<std::string, std::string>("-1", "-1");

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return std::pair<std::string, std::string>("No Saved Progress", "No Saved Progress") ; // display an alert saying you dont have any progress recorded
    
    std::pair<std::string, std::string> toReturnPair{"", ""};

    LevelStats myLevelStats = file::readJson(levelPath).value().as<LevelStats>();

    std::vector<std::pair<float,int>> deathsSorted;
    std::vector<std::pair<Run,int>> runsSorted;

    for (auto i : myLevelStats.deaths){
        float deathPFloat = std::stof(i.first);

        deathsSorted.push_back(std::pair<float,int>(deathPFloat, i.second));

        std::pair<float,int> prevOne;

        int currentMEIndex = deathsSorted.size() - 1;

        for (int b = 0; b < deathsSorted.size(); b++)
        {
            if (currentMEIndex != 0){
                if (deathsSorted[currentMEIndex - 1].first > deathsSorted[b].first){
                    prevOne = deathsSorted[currentMEIndex - 1];
                    deathsSorted[currentMEIndex - 1] = deathsSorted[b];
                    deathsSorted[currentMEIndex] = prevOne;
                }
            }
            currentMEIndex--;
        }
    }

    for (int i = 0; i < deathsSorted.size(); i++)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(Mod::get()->getSettingValue<int64_t>("precision")) << deathsSorted[i].first;
        toReturnPair.first += fmt::format("{}% {}X", ss.str(), deathsSorted[i].second);
        if (i != deathsSorted.size() - 1) toReturnPair.first += "\n";
    }
	
	for (auto i : myLevelStats.runs){
        Run currRun = StatsManager::splitRunKey(i.first);

        runsSorted.push_back(std::pair<Run,int>(currRun, i.second));

        std::pair<Run,int> prevOne;

        int currentMEIndex = runsSorted.size() - 1;

        for (int b = 0; b < runsSorted.size(); b++)
        {
            if (currentMEIndex != 0){
                if (runsSorted[currentMEIndex - 1].first.start == runsSorted[b].first.start){
                    if (runsSorted[currentMEIndex - 1].first.end > runsSorted[b].first.end){
                        prevOne = runsSorted[currentMEIndex - 1];
                        runsSorted[currentMEIndex - 1] = runsSorted[b];
                        runsSorted[currentMEIndex] = prevOne;
                    }
                }
                else if (runsSorted[currentMEIndex - 1].first.start > runsSorted[b].first.start){
                    prevOne = runsSorted[currentMEIndex - 1];
                    runsSorted[currentMEIndex - 1] = runsSorted[b];
                    runsSorted[currentMEIndex] = prevOne;
                }
            }
            currentMEIndex--;
        }
    }

    for (int i = 0; i < runsSorted.size(); i++)
    {
        std::stringstream start;
        start << std::fixed << std::setprecision(Mod::get()->getSettingValue<int64_t>("precision")) << runsSorted[i].first.start;
        std::stringstream end;
        end << std::fixed << std::setprecision(Mod::get()->getSettingValue<int64_t>("precision")) << runsSorted[i].first.end;
        toReturnPair.second += fmt::format("{}% - {}% {}X", start.str(), end.str(), runsSorted[i].second);
        if (i != runsSorted.size() - 1) toReturnPair.second += "\n";
    }
    

    return toReturnPair;
}