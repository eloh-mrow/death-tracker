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

    m_SLayer = ScrollLayer::create({m_size.width / 1.25f, m_size.height / 1.45f});
    m_SLayer->setZOrder(1);
    m_SLayer->setPosition(winSize / 2 - m_SLayer->getContentSize() / 2);
    this->m_mainLayer->addChild(m_SLayer);

    auto ScrollLayerBG = CCScale9Sprite::create("square02_small.png");
    ScrollLayerBG->setContentSize(m_SLayer->getContentSize());
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
        menu_selector(DTPopup::TogglePassRate)
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
        menu_selector(DTPopup::ToggleSessions)
    );
    sessionsButton->setPosition({winSize.width / 2 + m_size.width / 10.322f, winSize.height / 2 - m_size.height / 2.474f});
    m_buttonMenu->addChild(sessionsButton);

    GetLevelStats();

    CreateDeathsStrings();

    m_DeathsLabel = SimpleTextArea::create("", "chatFont.fnt", 0.75f);
    m_DeathsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_DeathsLabel->setAnchorPoint({0.5f, 1});
    m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height});
    m_SLayer->m_contentLayer->addChild(m_DeathsLabel);

    refreshText(texts::Deaths);

    scheduleUpdate();

    return true;
}

void DTPopup::update(float delta){
    m_DeathsLabel->setVisible(true);
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

void DTPopup::CreateDeathsStrings(){
    if (!m_Level){
        m_DeathStrings = std::vector<std::pair<std::string, float>>{std::pair<std::string, float>("-1", 0)};
        return;
    }

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)){
        m_DeathStrings = std::vector<std::pair<std::string, float>>{std::pair<std::string, float>("No Saved Progress", 0)}; // display an alert saying you dont have any progress recorded
        return;
    }

    std::vector<float> m_passRates = {};
	int totalDeaths = 0;
    for (auto i : m_MyLevelStats.deaths)
    {
        totalDeaths += i.second;
    }
    

	int offset = m_Level->m_normalPercent.value() == 100
		? 1
		: 0;

    std::vector<std::pair<std::string, float>> toReturn{};

    std::vector<std::pair<float, std::pair<int, float>>> deathsSorted;
    std::vector<std::pair<Run, std::pair<int, float>>> runsSorted;

    for (auto i : m_MyLevelStats.deaths){
        float deathPFloat = std::stof(i.first);
                
        bool addNew = true;
        int precision = Mod::get()->getSettingValue<int64_t>("precision");
        for (int b = 0; b < deathsSorted.size(); b++){
            std::string a1 = StatsManager::toPercentStr(deathPFloat, precision, true);
            std::string b1 = StatsManager::toPercentStr(deathsSorted[b].first, precision, true);

            if (a1 == b1){
                deathsSorted[b].second.first += i.second;
                addNew = false;
            }
        }

        if (addNew){
            deathsSorted.push_back(std::pair<float,std::pair<int, float>>(deathPFloat, std::pair<int, float>(i.second, 0)));

            std::pair<float,std::pair<int, float>> prevOne;

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
        totalDeaths -= deathsSorted[i].second.first;

        float passCount = totalDeaths;
		deathsSorted[i].second.second = (passCount + offset) / (passCount + deathsSorted[i].second.first + offset) * 100;
    }

    for (int i = 0; i < deathsSorted.size(); i++)
    {
        std::string precent = StatsManager::toPercentStr(deathsSorted[i].first, Mod::get()->getSettingValue<int64_t>("precision"), true);

        std::pair<std::string, float> currentPair;
        
        for (auto newBest : m_MyLevelStats.newBests) 
        {
            std::string fixedBestPrecent = StatsManager::toPercentStr(std::stof(newBest), Mod::get()->getSettingValue<int64_t>("precision"), true);
                        
            if (precent == fixedBestPrecent){
                currentPair.first += "<cy>";
            }
        }  

        currentPair.first += fmt::format("{}% {}X", precent, deathsSorted[i].second.first);
        currentPair.second = deathsSorted[i].second.second;

        toReturn.push_back(currentPair);
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
                runsSorted[b].second.first += i.second;
                addNew = false;
            }
        }

        if (addNew){
            runsSorted.push_back(std::pair<Run, std::pair<int, float>>(currRun, std::pair<int, float>(i.second, 0)));

            std::pair<Run,std::pair<int, float>> prevOne;

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

        std::pair<std::string, float> currentPair;

        currentPair.first += fmt::format("{}% - {}% {}X", start, end, runsSorted[i].second.first);
        currentPair.second = 0;
        toReturn.push_back(currentPair);
    }
    
    if (toReturn.size() == 0)
        m_DeathStrings = std::vector<std::pair<std::string, float>>{std::pair<std::string, float>("No Saved Progress", 0)};

    m_DeathStrings = toReturn;
}

void DTPopup::CopyText(CCObject* sender){
    clipboard::write(m_DeathsLabel->getText());
    Notification::create(
			std::string("Copied successfully"),
			CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png")
		)->show();
}

void DTPopup::TogglePassRate(CCObject* sender){
    if (m_passRateButtonIconInactive->isVisible()){
        m_passRateButtonIconInactive->setVisible(false);
        m_passRateButtonIconActive->setVisible(true);

        if (m_SessionsButtonIconInactive->isVisible())
            refreshText(texts::DeathsPassRate);
        else{
            refreshText(texts::SessionsPassRate);
        }
    }
    else{
        m_passRateButtonIconInactive->setVisible(true);
        m_passRateButtonIconActive->setVisible(false);

        if (m_SessionsButtonIconInactive->isVisible())
            refreshText(texts::Deaths);
        else{
            refreshText(texts::Sessions);
        }
    }
}

void DTPopup::ToggleSessions(CCObject* sender){
    if (m_SessionsButtonIconInactive->isVisible()){
        m_SessionsButtonIconInactive->setVisible(false);
        m_SessionsButtonIconActive->setVisible(true);

        if (m_passRateButtonIconInactive->isVisible())
            refreshText(texts::Sessions);
        else{
            refreshText(texts::SessionsPassRate);
        }
    }
    else{
        m_SessionsButtonIconInactive->setVisible(true);
        m_SessionsButtonIconActive->setVisible(false);

        if (m_passRateButtonIconInactive->isVisible())
            refreshText(texts::Deaths);
        else{
            refreshText(texts::DeathsPassRate);
        }
    }
}

void DTPopup::refreshText(texts textID){
    //set to deaths
    if (textID == texts::Deaths){
        this->setTitle("Deaths");
        if (m_DeathStrings[0].first != "-1"){
            if (m_DeathStrings[0].first == "No Saved Progress"){
                log::info("didnt find progress");
            }
            else{
                std::string mergedString = "";
                for (int i = 0; i < m_DeathStrings.size(); i++)
                {
                    mergedString += m_DeathStrings[i].first;
                    if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                }

                m_DeathsLabel->setText(mergedString);

                if (m_DeathsLabel->getContentSize().height > m_SLayer->getContentSize().height){
                    m_SLayer->m_contentLayer->setContentSize({m_SLayer->m_contentLayer->getContentSize().width, m_DeathsLabel->getContentSize().height});
                    m_DeathsLabel->setPositionY(m_DeathsLabel->getPositionY() + (m_SLayer->m_contentLayer->getContentSize().height - m_SLayer->getContentSize().height));                    
                }
                    
                m_SLayer->moveToTop();
            }
        }
    }
    //set to deaths with pass rate
    else if (textID == texts::DeathsPassRate){
        this->setTitle("Deaths Pass Rate");
        if (m_DeathStrings[0].first != "-1"){
            if (m_DeathStrings[0].first == "No Saved Progress"){
                log::info("didnt find progress");
            }
            else{
                std::string mergedString = "";
                for (int i = 0; i < m_DeathStrings.size(); i++)
                {
                    mergedString += m_DeathStrings[i].first;
                    if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                }
                
                m_DeathsLabel->setText(mergedString);

                if (m_DeathsLabel->getContentSize().height > m_SLayer->getContentSize().height){
                    m_SLayer->m_contentLayer->setContentSize({m_SLayer->m_contentLayer->getContentSize().width, m_DeathsLabel->getContentSize().height});
                    m_DeathsLabel->setPositionY(m_DeathsLabel->getPositionY() + (m_SLayer->m_contentLayer->getContentSize().height - m_SLayer->getContentSize().height));                    
                }
                    
                m_SLayer->moveToTop();
            }
        }
    }
    //set to session
    else if (textID == texts::Sessions){
        this->setTitle("Session");
    }
    //set to session with pass rate
    else if (textID == texts::SessionsPassRate){
        this->setTitle("Session Pass Rate");
    }

    //set coloring
    for (int i = 0; i < m_DeathsLabel->getLines().size(); i++)
    {
        bool didColorText = false;
        std::string currentTextStr = "";
        if (m_DeathsLabel->getLines()[i]->getString() != "")
            currentTextStr = m_DeathsLabel->getLines()[i]->getString();

        if (currentTextStr.length() >= 3){
            if (currentTextStr[0] == '<' && currentTextStr[1] == 'c' && currentTextStr[2] == 'y' && currentTextStr[3] == '>'){
                m_DeathsLabel->getLines()[i]->setColor({ 255, 255, 0 });
                didColorText = true;
            }

            if (currentTextStr[0] == '<' && currentTextStr[1] == 'c' && currentTextStr[2] == 'o' && currentTextStr[3] == '>'){
                m_DeathsLabel->getLines()[i]->setColor({ 236, 126, 0 });
                didColorText = true;
            }
        }

        if (!didColorText)
            m_DeathsLabel->getLines()[i]->setColor({255, 255, 255});
        else{
            currentTextStr = currentTextStr.erase(0, 4);
            m_DeathsLabel->getLines()[i]->setString(currentTextStr.c_str());
        }
    }
    
}