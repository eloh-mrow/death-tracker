#include "../layers/DTPopup.hpp"
#include "../managers/DTPopupManager.hpp"
#include "../utils/Settings.hpp"

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
    auto copyButtonSprite = ButtonSprite::create("Copy");
    copyButtonSprite->setScale(0.7f);
    auto copyButton = CCMenuItemSpriteExtra::create(
        copyButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::CopyText)
    );

    //passRate button
    auto passRateButtonSprite = ButtonSprite::create("", 15.f, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);
    passRateButtonSprite->setScale(0.7f);

    m_passRateButtonIconInactive = CCSprite::create("dt_passRateIcon_inactive.png"_spr);
    m_passRateButtonIconInactive->setScale(0.825f);
    m_passRateButtonIconInactive->setPosition(passRateButtonSprite->getContentSize() / 2);
    passRateButtonSprite->addChild(m_passRateButtonIconInactive);

    m_passRateButtonIconActive = CCSprite::create("dt_passRateIcon_active.png"_spr);
    m_passRateButtonIconActive->setScale(0.825f);
    m_passRateButtonIconActive->setPosition(passRateButtonSprite->getContentSize() / 2);
    m_passRateButtonIconActive->setVisible(false);
    passRateButtonSprite->addChild(m_passRateButtonIconActive);


    auto passRateButton = CCMenuItemSpriteExtra::create(
        passRateButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::TogglePassRate)
    );

    //session button
    auto sessionsButtonSprite = ButtonSprite::create("", 15.f, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);
    sessionsButtonSprite->setScale(0.7f);

    m_SessionsButtonIconInactive = CCSprite::create("dt_sessionIcon_inactive.png"_spr);
    m_SessionsButtonIconInactive->setScale(0.825f);
    m_SessionsButtonIconInactive->setPosition(sessionsButtonSprite->getContentSize() / 2);
    sessionsButtonSprite->addChild(m_SessionsButtonIconInactive);

    m_SessionsButtonIconActive = CCSprite::create("dt_sessionIcon_active.png"_spr);
    m_SessionsButtonIconActive->setScale(0.825f);
    m_SessionsButtonIconActive->setPosition(sessionsButtonSprite->getContentSize() / 2);
    m_SessionsButtonIconActive->setVisible(false);
    sessionsButtonSprite->addChild(m_SessionsButtonIconActive);

    auto sessionsButton = CCMenuItemSpriteExtra::create(
        sessionsButtonSprite,
        nullptr,
        this,
        menu_selector(DTPopup::ToggleSessions)
    );

    // bottom buttons menu
    auto bottomButtonsMenu = CCMenu::create();
    bottomButtonsMenu->addChild(copyButton);
    bottomButtonsMenu->addChild(sessionsButton);
    bottomButtonsMenu->addChild(passRateButton);

    bottomButtonsMenu->setPosition({
        winSize.width / 2,
        winSize.height / 2 - m_size.height / 2 + sessionsButton->getContentSize().height + 2
    });

    auto bottomButtonsMenuLayout = RowLayout::create();
    bottomButtonsMenuLayout->setGap(7.5f);
    bottomButtonsMenu->setLayout(bottomButtonsMenuLayout);
    this->m_mainLayer->addChild(bottomButtonsMenu);

    // ================================== //
    // loading data
    m_MyLevelStats = StatsManager::getLevelStats(m_Level);
    m_noSavedData = !m_MyLevelStats.deaths.size() && !m_MyLevelStats.runs.size();

    // log::info("m_MyLevelStats --\ndeaths.size() = {}\nruns.size() = {}\nnewBests.size() = {}\ncurrentBest = {}\nsessions.size() = {}",
    //     m_MyLevelStats.deaths.size(),
    //     m_MyLevelStats.runs.size(),
    //     m_MyLevelStats.newBests.size(),
    //     m_MyLevelStats.currentBest,
    //     m_MyLevelStats.sessions.size()
    // );

    m_DeathStrings = CreateDeathsString(m_MyLevelStats.deaths, m_MyLevelStats.newBests, "<cy>");
    m_RunStrings = CreateRunsString(m_MyLevelStats.runs);

    auto session = StatsManager::getSession();
    m_SessionStrings = CreateDeathsString(session->deaths, session->newBests, "<co>");
    m_SessionRunStrings = CreateRunsString(session->runs);
    // ================================== //

    m_DeathsLabel = SimpleTextArea::create("", "chatFont.fnt", 0.75f);
    m_DeathsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_DeathsLabel->setAnchorPoint({0.5f, 1});
    m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height});
    m_SLayer->m_contentLayer->addChild(m_DeathsLabel);

    m_PlatformerInfoCont = CCNode::create();
    m_SLayer->m_contentLayer->addChild(m_PlatformerInfoCont);


    //creates a graph for a deaths string

    
    auto graph = CreateGraph(m_DeathStrings, GetBestRun(m_MyLevelStats.newBests), {8, 0.5f});

    if (graph){
        this->addChild(graph);
    }
    

    if (!m_Level->isPlatformer())
        refreshText(texts::deaths);
    else{
        refreshPlatformerRuns(texts::deaths);
    }

    scheduleUpdate();

    return true;
}

void DTPopup::update(float delta){
    m_DeathsLabel->setVisible(true);
    m_PlatformerInfoCont->setVisible(true);
}

void DTPopup::onClose(cocos2d::CCObject* object){
    m_InfoAlertLayer->m_mainLayer->setVisible(true);
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

std::vector<std::tuple<std::string, int, float>> DTPopup::CreateDeathsString(Deaths deaths, NewBests newBests, std::string NewBestsColorString){
    if (!m_Level) return std::vector<std::tuple<std::string, int, float>>{std::tuple<std::string, int, float>("-1", 0, 0)};
    if (!deaths.size()) return std::vector<std::tuple<std::string, int, float>>{std::tuple<std::string, int, float>("No Saved Progress", 0, 0)};

    int totalDeaths = 0;
    std::vector<std::tuple<std::string, int>> sortedDeaths{};

    // sort the deaths
    for (const auto [percentKey, count] : deaths) {
        sortedDeaths.push_back(std::make_tuple(percentKey, count));
        totalDeaths += count;
    }

    std::ranges::sort(sortedDeaths, [](const std::tuple<std::string, int> a, const std::tuple<std::string, int> b) {
        auto percentA = std::stof(std::get<0>(a));
        auto percentB = std::stof(std::get<0>(b));
        return percentA < percentB; // true --> A before B
    });

    // create output
	int offset = m_Level->m_normalPercent.value() == 100
		? 1
		: 0;

    std::vector<std::tuple<std::string, int, float>> output{};

    for (const auto [percentKey, count] : sortedDeaths) {
        // calculate pass rate
        totalDeaths -= count;

        float passCount = totalDeaths;
        float passRate = (passCount + offset) / (passCount + count + offset) * 100;

        // format output
        auto labelStr = fmt::format("{}%", percentKey);

        if (newBests.contains(std::stoi(percentKey)))
            labelStr.insert(0, NewBestsColorString);

        output.push_back(std::tuple<std::string, int, float>(labelStr, count, passRate));
    }

    if (!output.size()) output.push_back(std::tuple<std::string, int, float>("No Saved Progress", 0, 0));
    return output;
}

std::vector<std::tuple<std::string, int>> DTPopup::CreateRunsString(Runs runs){
    if (!m_Level) return std::vector<std::tuple<std::string, int>>{std::tuple<std::string, int>("-1", 0)};
    if (!runs.size()) return std::vector<std::tuple<std::string, int>>{std::tuple<std::string, int>("No Saved Progress", 0)};

    std::vector<std::tuple<std::string, int>> sortedRuns{};

    for (const auto [runKey, count] : runs)
        sortedRuns.push_back(std::make_tuple(runKey, count));

    // sort the runs
    std::ranges::sort(sortedRuns, [](const std::tuple<std::string, int> a, const std::tuple<std::string, int> b) {
        auto runA = StatsManager::splitRunKey(std::get<0>(a));
        auto runB = StatsManager::splitRunKey(std::get<0>(b));

        // start is equal, compare end
        if (runA.start == runB.start) return runA.end < runB.end;
        return runA.start < runB.start;
    });

    // create output
    std::vector<std::tuple<std::string, int>> output{};

    for (const auto [runKey, count] : sortedRuns) {
        auto run = StatsManager::splitRunKey(runKey);
        auto labelStr = fmt::format("{}% - {}%", run.start, run.end);
        output.push_back(std::tuple<std::string, int>(labelStr, count));
    }

    return output;
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
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::DeathsPassRate);
            else
                refreshText(texts::DeathsPassRate);
        else{
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::SessionsPassRate);
            else
                refreshText(texts::SessionsPassRate);
        }
    }
    else{
        m_passRateButtonIconInactive->setVisible(true);
        m_passRateButtonIconActive->setVisible(false);

        if (m_SessionsButtonIconInactive->isVisible())
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::deaths);
            else
                refreshText(texts::deaths);
        else{
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::Sessions);
            else
                refreshText(texts::Sessions);
        }
    }
}

void DTPopup::ToggleSessions(CCObject* sender){
    if (m_SessionsButtonIconInactive->isVisible()){
        m_SessionsButtonIconInactive->setVisible(false);
        m_SessionsButtonIconActive->setVisible(true);

        if (m_passRateButtonIconInactive->isVisible())
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::Sessions);
            else
                refreshText(texts::Sessions);

        else{
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::SessionsPassRate);
            else
                refreshText(texts::SessionsPassRate);

        }
    }
    else{
        m_SessionsButtonIconInactive->setVisible(true);
        m_SessionsButtonIconActive->setVisible(false);

        if (m_passRateButtonIconInactive->isVisible())
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::deaths);
            else
                refreshText(texts::deaths);

        else{
            if (m_Level->isPlatformer())
                refreshPlatformerRuns(texts::DeathsPassRate);
            else
                refreshText(texts::DeathsPassRate);
        }
    }
}

void DTPopup::refreshText(texts textID){
    m_CurrentPage = textID;
    if (!m_Level) return;

    if (!m_noSavedData){
        if (textID == texts::deaths){
            this->setTitle("Deaths");
            if (std::get<0>(m_DeathStrings[0]) != "-1"/* && std::get<0>(m_RunStrings[0]) != "-1"*/){
                if (std::get<0>(m_DeathStrings[0]) == "No Saved Progress"/* && std::get<0>(m_RunStrings[0]) == "No Saved Progress"*/){
                    m_DeathsLabel->setText(std::get<0>(m_DeathStrings[0]));
                }
                else{
                    std::string mergedString = "";
                    if (std::get<0>(m_DeathStrings[0]) != "No Saved Progress"){
                        for (int i = 0; i < m_DeathStrings.size(); i++)
                        {
                            mergedString += fmt::format("{} x{}", std::get<0>(m_DeathStrings[i]), std::get<1>(m_DeathStrings[i]));
                            if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    // if (std::get<0>(m_RunStrings[0]) != "No Saved Progress")
                    //     for (int i = 0; i < m_RunStrings.size(); i++)
                    //     {
                    //         mergedString += fmt::format("{} x{}", std::get<0>(m_RunStrings[i]), std::get<1>(m_RunStrings[i]));
                    //         if (i != m_RunStrings.size() - 1) mergedString += '\n';
                    //     }

                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to deaths with pass rate
        else if (textID == texts::DeathsPassRate){
            this->setTitle("Deaths Pass Rate");
            if (std::get<0>(m_DeathStrings[0]) != "-1"/* && std::get<0>(m_RunStrings[0]) != "-1"*/){
                if (std::get<0>(m_DeathStrings[0]) == "No Saved Progress"/* && std::get<0>(m_RunStrings[0]) == "No Saved Progress"*/){
                    m_DeathsLabel->setText(std::get<0>(m_DeathStrings[0]));
                }
                else{
                    std::string mergedString = "";
                    if (std::get<0>(m_DeathStrings[0]) != "No Saved Progress"){
                        for (int i = 0; i < m_DeathStrings.size(); i++)
                        {
                            mergedString += fmt::format("{} x{} ({:.2f}%)", std::get<0>(m_DeathStrings[i]), std::get<1>(m_DeathStrings[i]), std::get<2>(m_DeathStrings[i]));
                            if (i != m_DeathStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    // if (std::get<0>(m_RunStrings[0]) != "No Saved Progress")
                    //     for (int i = 0; i < m_RunStrings.size(); i++)
                    //     {
                    //         mergedString += fmt::format("{} x{}", std::get<0>(m_RunStrings[i]), std::get<1>(m_RunStrings[i]));
                    //         if (i != m_RunStrings.size() - 1) mergedString += '\n';
                    //     }

                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to session
        else if (textID == texts::Sessions){
            this->setTitle("Session");
            if (std::get<0>(m_SessionStrings[0]) != "-1"/* && std::get<0>(m_SessionRunStrings[0]) != "-1"*/){
                if (std::get<0>(m_SessionStrings[0]) == "No Saved Progress"/* && std::get<0>(m_SessionRunStrings[0]) == "No Saved Progress"*/){
                    m_DeathsLabel->setText(std::get<0>(m_SessionStrings[0]));
                }
                else{
                    std::string mergedString = "";
                    if (std::get<0>(m_SessionStrings[0]) != "No Saved Progress"){
                        for (int i = 0; i < m_SessionStrings.size(); i++)
                        {
                            mergedString += fmt::format("{} x{}", std::get<0>(m_SessionStrings[i]), std::get<1>(m_SessionStrings[i]));
                            if (i != m_SessionStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    // if (std::get<0>(m_SessionRunStrings[0]) != "No Saved Progress")
                    //     for (int i = 0; i < m_SessionRunStrings.size(); i++)
                    //     {
                    //         mergedString += fmt::format("{} x{}", std::get<0>(m_SessionRunStrings[i]), std::get<1>(m_SessionRunStrings[i]));
                    //         if (i != m_SessionRunStrings.size() - 1) mergedString += '\n';
                    //     }

                    m_DeathsLabel->setText(mergedString);
                }
            }
        }
        //set to session with pass rate
        else if (textID == texts::SessionsPassRate){
            this->setTitle("Session Pass Rate");
            if (std::get<0>(m_SessionStrings[0]) != "-1"/* && std::get<0>(m_SessionRunStrings[0]) != "-1"*/){
                if (std::get<0>(m_SessionStrings[0]) == "No Saved Progress"/* && std::get<0>(m_SessionRunStrings[0]) == "No Saved Progress"*/){
                    m_DeathsLabel->setText(std::get<0>(m_SessionStrings[0]));
                }
                else{
                    std::string mergedString = "";
                    if (std::get<0>(m_SessionStrings[0]) != "No Saved Progress"){
                        for (int i = 0; i < m_SessionStrings.size(); i++)
                        {
                            mergedString += fmt::format("{} x{} ({:.2f}%)", std::get<0>(m_SessionStrings[i]), std::get<1>(m_SessionStrings[i]), std::get<2>(m_SessionStrings[i]));
                            if (i != m_SessionStrings.size() - 1) mergedString += '\n';
                        }

                        mergedString += '\n';
                    }

                    // if (std::get<0>(m_SessionRunStrings[0]) != "No Saved Progress")
                    //     for (int i = 0; i < m_SessionRunStrings.size(); i++)
                    //     {
                    //         mergedString += fmt::format("{} x{}", std::get<0>(m_SessionRunStrings[i]), std::get<1>(m_SessionRunStrings[i]));
                    //         if (i != m_SessionRunStrings.size() - 1) mergedString += '\n';
                    //     }

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

    float contentLayerHeight = 0;

    for (auto const& label : m_DeathsLabel->getLines()) {
        contentLayerHeight += label->getContentSize().height * m_DeathsLabel->getScale();
    }

    if (contentLayerHeight > m_SLayer->getContentSize().height){
        m_SLayer->m_contentLayer->setContentSize({
            m_SLayer->m_contentLayer->getContentSize().width,
            contentLayerHeight
        });

        m_DeathsLabel->setPosition({
            m_SLayer->getContentSize().width / 2,
            m_SLayer->getContentSize().height + (m_SLayer->m_contentLayer->getContentSize().height - m_SLayer->getContentSize().height)
        });
    }
    else {
        m_SLayer->m_contentLayer->setContentSize(m_SLayer->getContentSize());
        m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height});
    }

    if (m_DeathsLabel->getText() == "No Saved Progress") {
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

float DTPopup::GetBestRun(NewBests bests){
    int bestRun = 0;

    for (auto const& best : bests)
    {
        if (best > bestRun) bestRun = best;
    }

    return bestRun;
}

/*
    //
    creates a graph with a given deaths string
    change the scaling to change the space between the points on the x and y
    //
*/
CCNode* DTPopup::CreateGraph(std::vector<std::tuple<std::string, int, float>> deathsString, float bestRun, CCPoint Scaling){
    if (std::get<0>(deathsString[0]) == "-1" || std::get<0>(deathsString[0]) == "No Saved Progress") return nullptr;

    auto toReturnNode = CCNode::create();

    //calculate the points of the graph
    std::vector<CCPoint> drawPoints;

    for (int i = 0; i < 101; i++)
    {
        bool makePointB = true;
        for (int d = 0; d < deathsString.size(); d++)
        {
            std::string editedDeathString = std::get<0>(deathsString[d]);
            if (StatsManager::ContainsAtIndex(0, "<cy>", editedDeathString) || StatsManager::ContainsAtIndex(0, "<co>", editedDeathString)){
                editedDeathString = editedDeathString.erase(0, 4);
            }

            for (int b = 0; b < editedDeathString.length(); b++)
            {
                if (editedDeathString[b] == '%'){
                    editedDeathString.erase(b, editedDeathString.length() - b);
                    break;
                }
            }
            if (std::stof(editedDeathString) == i){
                drawPoints.push_back({static_cast<float>(i), std::get<2>(deathsString[d])});
                makePointB = false;
            }
        }

        if (i == 0 && makePointB){
            drawPoints.insert(drawPoints.begin(), {0, 100});
        }

        bool IBehind = false;
        bool IInfront = false;
        if (!makePointB){
            if (drawPoints.size() > 1){
                if (drawPoints[drawPoints.size() - 2].x != i - 1){
                    IBehind = true;
                }
            }
        }
        else{
            if (drawPoints.size() > 0)
                if (drawPoints[drawPoints.size() - 1].y != 100 && i > drawPoints[drawPoints.size() - 1].x){
                    if (i > bestRun){
                        drawPoints.insert(drawPoints.end(), {100, 0});
                    }
                    else{
                        IInfront = true;
                    }
                }
        }

        if (IBehind)
            drawPoints.insert(drawPoints.end() - 1, {static_cast<float>(i - 1), 100});

        if (IInfront)
            drawPoints.insert(drawPoints.end(), {static_cast<float>(i), 100});

    }

    //connect those points with lines
    CCPoint PrevPoint = {-1, -1};

    for (int i = 0; i < drawPoints.size(); i++)
    {
        CCPoint currPoint = {drawPoints[i].x * Scaling.x, drawPoints[i].y * Scaling.y};
        auto c = CCSprite::createWithSpriteFrameName("d_circle_02_001.png");
        c->setPosition(currPoint);
        c->setScale(0.05f);
        c->setZOrder(1);
        c->setColor({ 38, 255, 49 });
        toReturnNode->addChild(c);
        if (PrevPoint != ccp(-1, -1)){
            auto line = CCDrawNode::create();
            line->drawSegment(PrevPoint, currPoint, 1, { 38, 255, 49 });
            line->setCascadeColorEnabled(false);
            toReturnNode->addChild(line);
        }

        PrevPoint = currPoint;
    }

    return toReturnNode;
}

void DTPopup::refreshPlatformerRuns(texts textID){
    m_CurrentPage = textID;
    if (!m_Level) return;

    m_PlatformerInfoCont->removeMeAndCleanup();
    m_PlatformerInfoCont = CCNode::create();
    m_SLayer->m_contentLayer->addChild(m_PlatformerInfoCont);

    int amountAdded = 0;
    float hight = 32.f * 0.7f;

    if (!m_noSavedData){
        if (textID == texts::deaths){
            this->setTitle("Deaths");
            if (std::get<0>(m_DeathStrings[0]) != "-1" && std::get<0>(m_RunStrings[0]) != "-1"){
                if (std::get<0>(m_DeathStrings[0]) == "No Saved Progress" && std::get<0>(m_RunStrings[0]) == "No Saved Progress"){
                    refreshText(m_CurrentPage);
                }
                else{
                    for (int i = 0; i < m_DeathStrings.size(); i++)
                    {
                        auto contNode = createPlatformerDeath(std::get<0>(m_DeathStrings[i]), std::get<1>(m_DeathStrings[i]));

                        if (contNode){
                            m_PlatformerInfoCont->addChild(contNode);
                            contNode->setPositionY(-amountAdded * hight);
                            amountAdded++;
                        }
                    }
                }
            }
        }
        //set to deaths with pass rate
        else if (textID == texts::DeathsPassRate){
            this->setTitle("Deaths Pass Rate");
            if (std::get<0>(m_DeathStrings[0]) != "-1" && std::get<0>(m_RunStrings[0]) != "-1"){
                if (std::get<0>(m_DeathStrings[0]) == "No Saved Progress" && std::get<0>(m_RunStrings[0]) == "No Saved Progress"){
                    refreshText(m_CurrentPage);
                }
                else{
                    for (int i = 0; i < m_DeathStrings.size(); i++)
                    {
                        auto contNode = createPlatformerDeath(std::get<0>(m_DeathStrings[i]), std::get<1>(m_DeathStrings[i]), std::get<2>(m_DeathStrings[i]));

                        if (contNode){
                            m_PlatformerInfoCont->addChild(contNode);
                            contNode->setPositionY(-amountAdded * hight);
                            amountAdded++;
                        }
                    }
                }
            }
        }
        //set to session
        else if (textID == texts::Sessions){
            this->setTitle("Session");
            if (std::get<0>(m_SessionStrings[0]) != "-1" && std::get<0>(m_SessionRunStrings[0]) != "-1"){
                if (std::get<0>(m_SessionStrings[0]) == "No Saved Progress" && std::get<0>(m_SessionRunStrings[0]) == "No Saved Progress"){
                    refreshText(m_CurrentPage);
                }
                else{
                    for (int i = 0; i < m_SessionStrings.size(); i++)
                    {
                        auto contNode = createPlatformerDeath(std::get<0>(m_SessionStrings[i]), std::get<1>(m_SessionStrings[i]));

                        if (contNode){
                            m_PlatformerInfoCont->addChild(contNode);
                            contNode->setPositionY(-amountAdded * hight);
                            amountAdded++;
                        }
                    }
                }
            }
        }
        //set to session with pass rate
        else if (textID == texts::SessionsPassRate){
            this->setTitle("Session Pass Rate");
            if (std::get<0>(m_SessionStrings[0]) != "-1" && std::get<0>(m_SessionRunStrings[0]) != "-1"){
                if (std::get<0>(m_SessionStrings[0]) == "No Saved Progress" && std::get<0>(m_SessionRunStrings[0]) == "No Saved Progress"){
                    refreshText(m_CurrentPage);
                }
                else{
                    for (int i = 0; i < m_SessionStrings.size(); i++)
                    {
                        auto contNode = createPlatformerDeath(std::get<0>(m_SessionStrings[i]), std::get<1>(m_SessionStrings[i]), std::get<2>(m_SessionStrings[i]));

                        if (contNode){
                            m_PlatformerInfoCont->addChild(contNode);
                            contNode->setPositionY(-amountAdded * hight);
                            amountAdded++;
                        }
                    }
                }
            }
        }
    }
    else{
        //no save

    }

    float contentLayerHeight = amountAdded * hight;

    if (contentLayerHeight > m_SLayer->getContentSize().height){
        m_SLayer->m_contentLayer->setContentSize({
            m_SLayer->m_contentLayer->getContentSize().width,
            contentLayerHeight
        });

        m_PlatformerInfoCont->setPosition({
            m_SLayer->getContentSize().width / 2,
            m_SLayer->getContentSize().height + (m_SLayer->m_contentLayer->getContentSize().height - m_SLayer->getContentSize().height) - hight / 2
        });
    }
    else {
        m_SLayer->m_contentLayer->setContentSize(m_SLayer->getContentSize());
        m_PlatformerInfoCont->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height - hight / 2});
    }

    if (m_PlatformerInfoCont->getChildrenCount() == 0) {
        m_DeathsLabel->setText("No Saved Progress");
        m_DeathsLabel->setPosition({m_SLayer->getContentSize().width / 2, m_SLayer->getContentSize().height / 2 + m_DeathsLabel->getContentSize().height / 2});
    }
    else{
        m_DeathsLabel->setText("");
    }

    m_SLayer->moveToTop();

}

CCNode* DTPopup::createPlatformerDeath(std::string deathP, int Count, float passRate){
    if (deathP == "-1" || deathP == "No Saved Progress") return nullptr;
    auto contNode = CCNode::create();

    auto checkpointLabel = createCheckpointLabel(deathP);

    std::string countString = "x" + std::to_string(Count);
    if (passRate != -1)
        countString += fmt::format(" ({:.2f}%)", passRate);

    auto AmountLabel = CCLabelBMFont::create(countString.c_str(), "bigFont.fnt");
    AmountLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    AmountLabel->setPositionX(0);
    AmountLabel->setAnchorPoint({0, 0.5f});
    AmountLabel->setScale(0.4f);

    contNode->addChild(checkpointLabel.first);
    contNode->addChild(AmountLabel);

    if (passRate != -1)
        contNode->setPositionX((-AmountLabel->getContentSize().width / 3) * AmountLabel->getScale());

    if (checkpointLabel.second != -1)
        contNode->setPositionX(contNode->getPositionX() + checkpointLabel.second / 5);

    return contNode;
}

std::pair<CCNode*, float> DTPopup::createCheckpointLabel(std::string deathP){
    auto contNode = CCNode::create();

    std::string CheckNumLabelText = deathP;

    bool isRun = false;

    float lengthOfLabel = -1;

    //remove all '%'
    for (int i = 0; i < CheckNumLabelText.length(); i++)
    {
        if (CheckNumLabelText[i] == '%'){
            CheckNumLabelText = CheckNumLabelText.erase(i, 1);
            i--;
        }
        if (CheckNumLabelText[i] == '-')
            isRun = true;
    }

    if (!isRun){
        auto chackpSymble = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
        chackpSymble->setScale(0.7f);

        auto CheckNumLabel = CCLabelBMFont::create(CheckNumLabelText.c_str(), "bigFont.fnt");
        CheckNumLabel->setScale(0.35f);
        CheckNumLabel->setPosition(chackpSymble->getContentSize() / 2);
        chackpSymble->addChild(CheckNumLabel);

        bool didColorText = false;

        if (StatsManager::ContainsAtIndex(0, "<cy>", CheckNumLabelText)){
            CheckNumLabel->setColor({ 255, 255, 0 });
            didColorText = true;
        }

        if (StatsManager::ContainsAtIndex(0, "<co>", CheckNumLabelText)){
            CheckNumLabel->setColor({ 236, 126, 0 });
            didColorText = true;
        }

        if (!didColorText)
            CheckNumLabel->setColor({255, 255, 255});
        else{
            CheckNumLabelText = CheckNumLabelText.erase(0, 4);
            CheckNumLabel->setString(CheckNumLabelText.c_str());
        }

        contNode->addChild(chackpSymble);
        contNode->setPositionX(-10);
    }
    else{
        auto run = StatsManager::splitRunKey(CheckNumLabelText);

        auto chackpSymbleStart = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");

        auto CheckNumLabelStart = CCLabelBMFont::create(std::to_string(run.start).c_str(), "bigFont.fnt");
        CheckNumLabelStart->setScale(0.35f);
        CheckNumLabelStart->setPosition(chackpSymbleStart->getContentSize() / 2);
        chackpSymbleStart->addChild(CheckNumLabelStart);

        auto chackpSymbleEnd = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");

        auto CheckNumLabelEnd = CCLabelBMFont::create(std::to_string(run.end).c_str(), "bigFont.fnt");
        CheckNumLabelEnd->setScale(0.35f);
        CheckNumLabelEnd->setPosition(chackpSymbleEnd->getContentSize() / 2);
        chackpSymbleEnd->addChild(CheckNumLabelEnd);

        auto CheckNumLabel = CCLabelBMFont::create(" - ", "bigFont.fnt");
        CheckNumLabel->setScale(0.35f);

        CheckNumLabel->setPositionX(-chackpSymbleEnd->getContentSize().width);
        chackpSymbleStart->setPositionX(-CheckNumLabel->getContentSize().width / 1.5f - chackpSymbleEnd->getContentSize().width / 4);

        lengthOfLabel += CheckNumLabel->getContentSize().width + chackpSymbleStart->getContentSize().width + chackpSymbleEnd->getContentSize().width + 1;

        contNode->addChild(chackpSymbleEnd);
        contNode->addChild(CheckNumLabel);
        contNode->addChild(chackpSymbleStart);
        contNode->setPositionX(-10);
    }



    return std::pair<CCNode*, float>(contNode, lengthOfLabel);
}