#include "../layers/DTPopup.hpp"
#include "../managers/DTPopupManager.hpp"

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

    auto infoButtonSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoButtonSprite->setScale(0.7f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::ShowInfo)
    );
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition({winSize.width / 2 + m_size.width / 2.28f, winSize.height / 2 + m_size.height / 2.162f});

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

    if (GetLevelStats()){
        m_DeathStrings = CreateDeathsString(m_MyLevelStats.deaths, m_MyLevelStats.newBests, "<cy>");
        m_RunStrings = CreateRunsString(m_MyLevelStats.runs);

        if (m_MyLevelStats.sessions.size() > 0){
            Session latest = m_MyLevelStats.sessions[0];
            for (int i = 0; i < m_MyLevelStats.sessions.size(); i++)
            {
                if (m_MyLevelStats.sessions[i].lastPlayed > latest.lastPlayed){
                    latest = m_MyLevelStats.sessions[i];
                }
            }
            
            m_SessionStrings = CreateDeathsString(latest.deaths, latest.newBests, "<co>");
            m_SessionRunStrings = CreateRunsString(latest.runs);
        }
    }

    m_DeathsLabel = SimpleTextArea::create("", "chatFont.fnt", 0.75f);
    m_DeathsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_DeathsLabel->setAnchorPoint({0.5f, 1});
    m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height});
    m_SLayer->m_contentLayer->addChild(m_DeathsLabel);

    refreshText(texts::deaths);

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

bool DTPopup::GetLevelStats(){
    if (!m_Level) return false;

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return false;

    m_MyLevelStats = file::readJson(levelPath).value().as<LevelStats>();
    return true;
}

std::vector<std::pair<std::string, float>> DTPopup::CreateDeathsString(Deaths deaths, NewBests newBests, std::string NewBestsColorString){
    if (!m_Level) return std::vector<std::pair<std::string, float>>{std::pair<std::string, float>("-1", 0)};

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return std::vector<std::pair<std::string, float>>{std::pair<std::string, float>("No Saved Progress", 0)}; // display an alert saying you dont have any progress recorded

    std::vector<std::pair<std::string, float>> toReturn{};

    std::vector<std::pair<float, std::pair<int, float>>> deathsSorted;

    for (auto i : deaths){
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

    std::vector<float> m_passRates = {};
	int totalDeaths = 0;
    for (int i = 0; i < deathsSorted.size(); i++)
    {
        totalDeaths += deathsSorted[i].second.first;
    }
    

	int offset = m_Level->m_normalPercent.value() == 100
		? 1
		: 0;

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

        currentPair.first += fmt::format("{}% {}X", precent, deathsSorted[i].second.first);
        currentPair.second = deathsSorted[i].second.second;

        for (auto newBest : newBests)
        {
            std::string fixedBestPrecent = StatsManager::toPercentStr(std::stof(newBest), Mod::get()->getSettingValue<int64_t>("precision"), true);
            
            if (currentPair.first.length() >= 3)
                if (precent == fixedBestPrecent && !StatsManager::ContainsAtIndex(0, NewBestsColorString, currentPair.first)){
                    currentPair.first.insert(0, NewBestsColorString);
                }
        }  

        toReturn.push_back(currentPair);
    }

    if (toReturn.size() == 0)
        toReturn.push_back(std::pair<std::string, float>{"No Saved Progress", 0});

    return toReturn;
}

std::vector<std::string> DTPopup::CreateRunsString(Runs runs){
    if (!m_Level) return std::vector<std::string>{"-1", 0};

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (!ghc::filesystem::exists(levelPath)) return std::vector<std::string>{"No Saved Progress"}; // display an alert saying you dont have any progress recorded

    std::vector<std::string> toReturn{};

    std::vector<std::pair<Run, int>> runsSorted;

    for (auto i : runs){
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
            runsSorted.push_back(std::pair<Run, int>(currRun, i.second));

            std::pair<Run, int> prevOne;

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

        std::string currentPair;

        currentPair += fmt::format("{}% - {}% {}X", start, end, runsSorted[i].second);
        toReturn.push_back(currentPair);
    }
    
    if (toReturn.size() == 0)
        toReturn.push_back("No Saved Progress");

    return toReturn;
}

void DTPopup::CopyText(CCObject* sender){
    if (m_DeathsLabel->getText() == "No Saved Progress"){
        Notification::create(
			std::string("No progress to copy"),
			CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png")
		)->show();
        return;
    }

    std::string textToSend = m_DeathsLabel->getText();
    for (int i = 0; i < textToSend.length(); i++)
    {
        if (StatsManager::ContainsAtIndex(i, "<cy>", textToSend) || StatsManager::ContainsAtIndex(i, "<co>", textToSend)){
            textToSend = textToSend.erase(i, 4);
        }
    }

    if (textToSend[textToSend.length() - 1] == '\n') textToSend = textToSend.erase(textToSend.length() - 1, 1);

    clipboard::write(textToSend);
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
            refreshText(texts::deaths);
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
            refreshText(texts::deaths);
        else{
            refreshText(texts::DeathsPassRate);
        }
    }
}

void DTPopup::refreshText(texts textID){
    m_CurrentPage = textID;
    if (!m_Level) return;

    auto levelPath = StatsManager::getLevelSaveFilePath(m_Level);

    if (ghc::filesystem::exists(levelPath)){
        if (textID == texts::deaths){
            this->setTitle("Deaths");
            if (m_DeathStrings[0].first != "-1" && m_RunStrings[0] != "-1"){
                if (m_DeathStrings[0].first == "No Saved Progress" && m_RunStrings[0] == "No Saved Progress"){
                    m_DeathsLabel->setText(m_DeathStrings[0].first);
                }
                else{
                    std::string mergedString = "";
                    if (m_DeathStrings[0].first != "No Saved Progress"){
                        for (int i = 0; i < m_DeathStrings.size(); i++)
                        {
                            mergedString += m_DeathStrings[i].first;
                            if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    if (m_RunStrings[0] != "No Saved Progress")
                        for (int i = 0; i < m_RunStrings.size(); i++)
                        {
                            mergedString += m_RunStrings[i];
                            if (i != m_RunStrings.size() - 1) mergedString += '\n';
                        }
                    
                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to deaths with pass rate
        else if (textID == texts::DeathsPassRate){
            this->setTitle("Deaths Pass Rate");
            if (m_DeathStrings[0].first != "-1" && m_RunStrings[0] != "-1"){
                if (m_DeathStrings[0].first == "No Saved Progress" && m_RunStrings[0] == "No Saved Progress"){
                    m_DeathsLabel->setText(m_DeathStrings[0].first);
                }
                else{
                    std::string mergedString = "";
                    if (m_DeathStrings[0].first != "No Saved Progress"){
                        for (int i = 0; i < m_DeathStrings.size(); i++)
                        {
                            mergedString += m_DeathStrings[i].first + fmt::format(" ({:.2f}%)",m_DeathStrings[i].second);
                            if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    if (m_RunStrings[0] != "No Saved Progress")
                        for (int i = 0; i < m_RunStrings.size(); i++)
                        {
                            mergedString += m_RunStrings[i];
                            if (i != m_RunStrings.size() - 1) mergedString += '\n';
                        }
                    
                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to session
        else if (textID == texts::Sessions){
            this->setTitle("Session");
            if (m_SessionStrings[0].first != "-1" && m_SessionRunStrings[0] != "-1"){
                if (m_SessionStrings[0].first == "No Saved Progress" && m_SessionRunStrings[0] == "No Saved Progress"){
                    m_DeathsLabel->setText(m_SessionStrings[0].first);
                }
                else{
                    std::string mergedString = "";
                    if (m_SessionStrings[0].first != "No Saved Progress"){
                        for (int i = 0; i < m_SessionStrings.size(); i++)
                        {
                            mergedString += m_SessionStrings[i].first;
                            if (i != m_SessionStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    if (m_SessionRunStrings[0] != "No Saved Progress")
                        for (int i = 0; i < m_SessionRunStrings.size(); i++)
                        {
                            mergedString += m_SessionRunStrings[i];
                            if (i != m_SessionRunStrings.size() - 1) mergedString += '\n';
                        }
                    
                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to session with pass rate
        else if (textID == texts::SessionsPassRate){
            this->setTitle("Session Pass Rate");
            if (m_SessionStrings[0].first != "-1" && m_SessionRunStrings[0] != "-1"){
                if (m_SessionStrings[0].first == "No Saved Progress" && m_SessionRunStrings[0] == "No Saved Progress"){
                    m_DeathsLabel->setText(m_SessionStrings[0].first);
                }
                else{
                    std::string mergedString = "";
                    if (m_SessionStrings[0].first != "No Saved Progress"){
                        for (int i = 0; i < m_SessionStrings.size(); i++)
                        {
                            mergedString += m_SessionStrings[i].first + fmt::format(" ({:.2f}%)",m_SessionStrings[i].second);
                            if (i != m_SessionStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    if (m_SessionRunStrings[0] != "No Saved Progress")
                        for (int i = 0; i < m_SessionRunStrings.size(); i++)
                        {
                            mergedString += m_SessionRunStrings[i];
                            if (i != m_SessionRunStrings.size() - 1) mergedString += '\n';
                        }
                    
                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
    }
    else{
        m_DeathsLabel->setText("No Saved Progress");
        if (textID == texts::deaths){
            this->setTitle("Deaths");
        }
        //set to deaths with pass rate
        else if (textID == texts::DeathsPassRate){
            this->setTitle("Deaths Pass Rate");
        }
        //set to session
        else if (textID == texts::Sessions){
            this->setTitle("Session");
        }
        //set to session with pass rate
        else if (textID == texts::SessionsPassRate){
            this->setTitle("Session Pass Rate");
        }
    }

    if (m_DeathsLabel->getContentSize().height > m_SLayer->getContentSize().height){
        m_SLayer->m_contentLayer->setContentSize({m_SLayer->m_contentLayer->getContentSize().width, m_DeathsLabel->getContentSize().height});
        m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height + (m_SLayer->m_contentLayer->getContentSize().height - m_SLayer->getContentSize().height)});                  
    }
    else{
        m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height});
    }

    if (m_DeathsLabel->getText() == "No Saved Progress"){
        m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height / 2 + m_DeathsLabel->getContentSize().height / 2});
    }
                    
    m_SLayer->moveToTop();

    //set coloring
    for (int i = 0; i < m_DeathsLabel->getLines().size(); i++)
    {
        bool didColorText = false;
        std::string currentTextStr = "";
        if (m_DeathsLabel->getLines()[i]->getString() != "")
            currentTextStr = m_DeathsLabel->getLines()[i]->getString();

        if (StatsManager::ContainsAtIndex(0, "<cy>", currentTextStr)){
            m_DeathsLabel->getLines()[i]->setColor({ 255, 255, 0 });
            didColorText = true;
        }

        if (StatsManager::ContainsAtIndex(0, "<co>", currentTextStr)){
            m_DeathsLabel->getLines()[i]->setColor({ 236, 126, 0 });
            didColorText = true;
        }

        if (!didColorText)
            m_DeathsLabel->getLines()[i]->setColor({255, 255, 255});
        else{
            currentTextStr = currentTextStr.erase(0, 4);
            m_DeathsLabel->getLines()[i]->setString(currentTextStr.c_str());
        }
    }
    
}

const std::string INFO_ALERT_MESSAGES[8]{
	// side scroll, total, deaths
	"Percentages in <cy>yellow</c> indicate a new best.",

	// side scroll, total, pass rate
	"Pass rate indicates how likely you are to pass a percentage.\n\nPercentages in <cy>yellow</c> indicate a new best.",

	// side scroll, session, deaths
	"Percentages in <co>orange</c> indicate a new best for your current session.",

	// side scroll, session, pass rate
	"Session pass rate indicates how likely you are to pass a percentage for your current session.\n\nPercentages in <co>orange</c> indicate a new best for your current session.",

	// platformer, total, deaths
	"Checkpoints in <cy>yellow</c> indicate a new best.",

	// platformer, total, pass rate
	"Pass rate indicates how likely you are to pass a checkpoint.\n\nCheckpoints in <cy>yellow</c> indicate a new best.",

	// platformer, session, deaths
	"Checkpoints in <co>orange</c> indicate a new best for your current session.",

	// platformer, session, pass rate
	"Pass rate indicates how likely you are to pass a checkpoint for your current session.\n\nCheckpoints in <co>orange</c> indicate a new best for your current session."
};

void DTPopup::ShowInfo(CCObject* sender){
    DTPopupManager::setInfoAlertOpen(false);
    auto alert = FLAlertLayer::create("info", INFO_ALERT_MESSAGES[static_cast<int>(m_CurrentPage)], "OK");
    alert->show();
    DTPopupManager::setInfoAlertOpen(true);
}