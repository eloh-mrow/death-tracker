#include "../layers/DTPopup.hpp"

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

    //copy button
    auto copyButtonSprite = CCSprite::createWithSpriteFrameName("GJ_longBtn01_001.png");
    copyButtonSprite->setScale(0.7f);
    auto copyButtonText = CCLabelBMFont::create("Copy", "goldFont.fnt");
    copyButtonText->setAnchorPoint({0.5f, 0.4f});
    copyButtonText->setPosition(copyButtonSprite->getContentSize() / 2);
    copyButtonSprite->addChild(copyButtonText);
    auto copyButton = CCMenuItemSpriteExtra::create(
        copyButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::CopyText)
    );
    copyButton->setPosition({winSize.width / 2 - m_size.width / 4.923f, winSize.height / 2 - m_size.height / 2.474f});
    m_buttonMenu->addChild(copyButton);

    //passRate button
    auto passRateButtonSprite = CCSprite::create("GJ_button_01.png");
    passRateButtonSprite->setScale(0.6f);

    m_passRateButtonIconInactive = CCSprite::create("dt_passRateIcon_inactive.png"_spr);
    m_passRateButtonIconInactive->setPosition(passRateButtonSprite->getContentSize() / 2);
    passRateButtonSprite->addChild(m_passRateButtonIconInactive);

    m_passRateButtonIconActive = CCSprite::create("dt_passRateIcon_active.png"_spr);
    m_passRateButtonIconActive->setPosition(passRateButtonSprite->getContentSize() / 2);
    m_passRateButtonIconActive->setVisible(false);
    passRateButtonSprite->addChild(m_passRateButtonIconActive);

    auto passRateButton = CCMenuItemSpriteExtra::create(
        passRateButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::CopyText)
    );
    log::info("{}, {}", winSize, m_size);
    passRateButton->setPosition({winSize.width / 2 + m_size.width / 3.516f, winSize.height / 2 - m_size.height / 2.474f});
    m_buttonMenu->addChild(passRateButton);

    //session button
    auto sessionsButtonSprite = CCSprite::create("GJ_button_01.png");
    sessionsButtonSprite->setScale(0.6f);

    m_SessionsButtonIconInactive = CCSprite::create("dt_sessionIcon_inactive.png"_spr);
    m_SessionsButtonIconInactive->setPosition(sessionsButtonSprite->getContentSize() / 2);
    sessionsButtonSprite->addChild(m_SessionsButtonIconInactive);

    m_SessionsButtonIconActive = CCSprite::create("dt_sessionIcon_active.png"_spr);
    m_SessionsButtonIconActive->setPosition(sessionsButtonSprite->getContentSize() / 2);
    m_SessionsButtonIconActive->setVisible(false);
    sessionsButtonSprite->addChild(m_SessionsButtonIconActive);

    auto sessionsButton = CCMenuItemSpriteExtra::create(
        sessionsButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::CopyText)
    );
    sessionsButton->setPosition({winSize.width / 2 + m_size.width / 10.322f, winSize.height / 2 - m_size.height / 2.474f});
    m_buttonMenu->addChild(sessionsButton);

    GetLevelStats();

    auto deathStrings = CreateDeathsStrings();

    if (deathStrings.first != "-1"){
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

            for (int i = 0; i < DeathsLabel->getLines().size(); i++)
            {
                for (auto newBest : m_MyLevelStats.newBests) 
                {
                    std::string fixedBestPrecent = StatsManager::toPercentStr(std::stof(newBest), Mod::get()->getSettingValue<int64_t>("precision"), true);
                    std::string fixedLabelString = DeathsLabel->getLines()[i]->getString();
                    int PrecentIndex = 0;
                    for (int b = 0; b < fixedLabelString.length(); b++)
                    {
                        if (fixedLabelString[b] == '%')
                            PrecentIndex = b;
                    }
                    fixedLabelString = fixedLabelString.erase(PrecentIndex, fixedLabelString.length() - 1);
                    
                    if (fixedLabelString == fixedBestPrecent){
                        DeathsLabel->getLines()[i]->setColor({255, 255, 0});
                    }
                }  
            }
            

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

void DTPopup::GetLevelStats(){
    if (!m_Level) return;

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return; // display an alert saying you dont have any progress recorded

    m_MyLevelStats = file::readJson(levelPath).value().as<LevelStats>();
}

std::pair<std::string, std::string> DTPopup::CreateDeathsStrings(){
    if (!m_Level) return std::pair<std::string, std::string>("-1", "-1");

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return std::pair<std::string, std::string>("No Saved Progress", "No Saved Progress") ; // display an alert saying you dont have any progress recorded

    std::pair<std::string, std::string> toReturnPair{"", ""};

    std::vector<std::pair<float,int>> deathsSorted;
    std::vector<std::pair<Run,int>> runsSorted;

    for (auto i : m_MyLevelStats.deaths){
        float deathPFloat = std::stof(i.first);
                
        bool addNew = true;
        int precision = Mod::get()->getSettingValue<int64_t>("precision");
        for (int b = 0; b < deathsSorted.size(); b++){
            std::string a1 = StatsManager::toPercentStr(deathPFloat, precision, true);
            std::string b1 = StatsManager::toPercentStr(deathsSorted[b].first, precision, true);

            if (a1 == b1){
                deathsSorted[b].second += i.second;
                addNew = false;
            }
        }

        if (addNew){
            deathsSorted.push_back(std::pair<float,int>(deathPFloat, i.second));

            std::pair<float,int> prevOne;

            int currentMEIndex = deathsSorted.size() - 1;

            for (int b = 0; b < deathsSorted.size(); b++)
            {
                if (currentMEIndex != 0){
                    if (deathsSorted[currentMEIndex - 1].first > deathsSorted[currentMEIndex].first){
                        prevOne = deathsSorted[currentMEIndex - 1];
                        deathsSorted[currentMEIndex - 1] = deathsSorted[currentMEIndex];
                        deathsSorted[currentMEIndex] = prevOne;
                    }
                }
                currentMEIndex--;
            }
        }
    }

    for (int i = 0; i < deathsSorted.size(); i++)
    {
        std::string precent = StatsManager::toPercentStr(deathsSorted[i].first, Mod::get()->getSettingValue<int64_t>("precision"), true);
        

        toReturnPair.first += fmt::format("{}% {}X", precent, deathsSorted[i].second);

        if (i != deathsSorted.size() - 1) toReturnPair.first += "\n";
    }
	
	for (auto i : m_MyLevelStats.runs){
        Run currRun = StatsManager::splitRunKey(i.first);

        bool addNew = true;
        int precision = Mod::get()->getSettingValue<int64_t>("precision");
        for (int b = 0; b < runsSorted.size(); b++){
            std::string a1s = StatsManager::toPercentStr(currRun.start, precision, true);
            std::string a1e = StatsManager::toPercentStr(currRun.end, precision, true);

            std::string b1s = StatsManager::toPercentStr(runsSorted[b].first.start, precision, true);
            std::string b1e = StatsManager::toPercentStr(runsSorted[b].first.end, precision, true);

            if (a1s == b1s && a1e == b1e){
                runsSorted[b].second += i.second;
                addNew = false;
            }
        }

        if (addNew){
            runsSorted.push_back(std::pair<Run,int>(currRun, i.second));

            std::pair<Run,int> prevOne;

            int currentMEIndex = runsSorted.size() - 1;

            for (int b = 0; b < runsSorted.size(); b++)
            {
                if (currentMEIndex != 0){
                    if (runsSorted[currentMEIndex - 1].first.start == runsSorted[currentMEIndex].first.start){
                        if (runsSorted[currentMEIndex - 1].first.end > runsSorted[currentMEIndex].first.end){
                            prevOne = runsSorted[currentMEIndex - 1];
                            runsSorted[currentMEIndex - 1] = runsSorted[currentMEIndex];
                            runsSorted[currentMEIndex] = prevOne;
                        }
                    }
                    else if (runsSorted[currentMEIndex - 1].first.start > runsSorted[currentMEIndex].first.start){
                        prevOne = runsSorted[currentMEIndex - 1];
                        runsSorted[currentMEIndex - 1] = runsSorted[currentMEIndex];
                        runsSorted[currentMEIndex] = prevOne;
                    }
                }
                currentMEIndex--;
            }
        }
    }

    for (int i = 0; i < runsSorted.size(); i++)
    {
        std::string start = StatsManager::toPercentStr(runsSorted[i].first.start, Mod::get()->getSettingValue<int64_t>("precision"), true);
        std::string end = StatsManager::toPercentStr(runsSorted[i].first.end, Mod::get()->getSettingValue<int64_t>("precision"), true);

        toReturnPair.second += fmt::format("{}% - {}% {}X", start, end, runsSorted[i].second);
        if (i != runsSorted.size() - 1) toReturnPair.second += "\n";
    }
    

    return toReturnPair;
}

void DTPopup::CopyText(CCObject* sender){
    std::string strToCopy = "";
    for (int i = 0; i < m_TextAreas.size(); i++)
    {
        strToCopy += m_TextAreas[i]->getText();
        if (i != m_TextAreas.size() - 1) strToCopy += '\n';
    }
    clipboard::write(strToCopy);
    Notification::create(
			std::string("Copied successfully"),
			CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png")
		)->show();
}

