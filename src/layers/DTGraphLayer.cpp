#include "../layers/DTGraphLayer.hpp"
#include "../layers/ChooseRunCell.hpp"
#include "../utils/Settings.hpp"

DTGraphLayer* DTGraphLayer::create(DTLayer* const& layer) {
    auto ret = new DTGraphLayer();
    if (ret && ret->initAnchored(520, 280, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTGraphLayer::setup(DTLayer* const& layer) {

    m_DTLayer = layer;

    auto overallInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    overallInfoBS->setScale(0.8f);
    auto overallInfoButton = CCMenuItemSpriteExtra::create(
        overallInfoBS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::onOverallInfo)
    );
    overallInfoButton->setPosition(m_size.width - 8.5f, m_size.height - 8.5f);
    this->m_buttonMenu->addChild(overallInfoButton);

    alignmentNode = CCNode::create();
    alignmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(alignmentNode);

    noGraphLabel = CCLabelBMFont::create("No Progress\nFor Graph", "bigFont.fnt");
    noGraphLabel->setZOrder(1);
    noGraphLabel->setVisible(false);
    noGraphLabel->setPosition({46, 3});
    alignmentNode->addChild(noGraphLabel);

    refreshGraph();

    CCScale9Sprite* FontTextDisplayBG = CCScale9Sprite::create("square02b_001.png", {0,0, 80, 80});
    FontTextDisplayBG->setPosition({35, 0});
    FontTextDisplayBG->setContentSize({430, 256});
    FontTextDisplayBG->setColor({0,0,0});
    FontTextDisplayBG->setOpacity(125);
    alignmentNode->addChild(FontTextDisplayBG);

    CCScale9Sprite* InfoBG = CCScale9Sprite::create("square02b_001.png", {0,0, 80, 80});
    InfoBG->setPosition({-215, -85});
    InfoBG->setContentSize({65, 83});
    InfoBG->setColor({ 113, 167, 255 });
    InfoBG->setOpacity(78);
    alignmentNode->addChild(InfoBG);

    auto InfoLabel = CCLabelBMFont::create("Point Info", "bigFont.fnt");
    InfoLabel->setPosition({-215, -36});
    InfoLabel->setScale(0.35f);
    alignmentNode->addChild(InfoLabel);

    npsLabel = CCLabelBMFont::create("No\nPoint\nSelected", "bigFont.fnt");
    npsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    npsLabel->setScale(0.35f);
    npsLabel->setPosition({-215, -83});
    npsLabel->setZOrder(1);
    npsLabel->setVisible(false);
    npsLabel->setPositionY(npsLabel->getPositionY());
    alignmentNode->addChild(npsLabel);

    std::string typeText = "Passrate";

    if (currentType == GraphType::ReachRate)
        typeText = "Reachrate";

    PointInfoLabel = SimpleTextArea::create(fmt::format("Percent\n \nP{}:\nlol", typeText).c_str(), "bigFont.fnt");
    PointInfoLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    PointInfoLabel->setPosition({-215, -83});
    PointInfoLabel->setZOrder(1);
    PointInfoLabel->setVisible(false);
    PointInfoLabel->setScale(0.35f);
    alignmentNode->addChild(PointInfoLabel);

    auto SessionSelectCont = CCNode::create();
    SessionSelectCont->setID("Session-Select-Container");
    SessionSelectCont->setPosition({-215, 50});
    SessionSelectCont->setScale(0.85f);
    alignmentNode->addChild(SessionSelectCont);

    auto m_SessionSelectMenu = CCMenu::create();
    m_SessionSelectMenu->setPosition({0, 0});
    SessionSelectCont->addChild(m_SessionSelectMenu);

    std::ranges::sort(m_DTLayer->m_SharedLevelStats.sessions, [](const Session a, const Session b) {
        return a.lastPlayed > b.lastPlayed;
    });

    m_SessionSelectionInput = TextInput::create(120, "Session");
    if (m_DTLayer->m_SessionsAmount == 0)
        m_SessionSelectionInput->setString("No sessions.");
    else
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_DTLayer->m_SessionSelected, m_DTLayer->m_SessionsAmount));
    m_SessionSelectionInput->getInputNode()->setDelegate(this);
    m_SessionSelectionInput->setCommonFilter(CommonFilter::Uint);
    m_SessionSelectionInput->setScale(0.45f);
    SessionSelectCont->addChild(m_SessionSelectionInput);

    auto SessionSelectionRightS = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    SessionSelectionRightS->setScaleX(0.35f);
    SessionSelectionRightS->setScaleY(0.2f);
    auto SessionSelectionRight = CCMenuItemSpriteExtra::create(
        SessionSelectionRightS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::switchedSessionRight)
    );
    SessionSelectionRight->setPosition({34, 0});
    m_SessionSelectMenu->addChild(SessionSelectionRight);

    auto SessionSelectionLeftS = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    SessionSelectionLeftS->setScaleX(0.35f);
    SessionSelectionLeftS->setScaleY(0.2f);
    auto SessionSelectionLeft = CCMenuItemSpriteExtra::create(
        SessionSelectionLeftS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::switchedSessionLeft)
    );
    SessionSelectionLeft->setPosition({-34, 0});
    SessionSelectionLeft->setRotation(180);
    m_SessionSelectMenu->addChild(SessionSelectionLeft);

    auto SessionSelectionLabel = CCLabelBMFont::create("Session", "bigFont.fnt");
    SessionSelectionLabel->setPosition({0, 16});
    SessionSelectionLabel->setScale(0.45f);
    SessionSelectCont->addChild(SessionSelectionLabel);

    auto viewModeLabel = CCLabelBMFont::create("View Mode", "bigFont.fnt");
    viewModeLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    viewModeLabel->setScale(0.35f);
    viewModeLabel->setPosition({-215, 120});
    alignmentNode->addChild(viewModeLabel);

    viewModeButtonS = ButtonSprite::create("Normal");
    viewModeButtonS->setScale(0.45f);
    auto viewModeButton = CCMenuItemSpriteExtra::create(
        viewModeButtonS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::onViewModeButton)
    );
    viewModeButton->setPosition({-215 + m_size.width / 2, 106 + m_size.height / 2});
    this->m_buttonMenu->addChild(viewModeButton);

    runViewModeButtonS = ButtonSprite::create("From 0");
    runViewModeButtonS->setScale(0.45f);
    auto runViewModeButton = CCMenuItemSpriteExtra::create(
        runViewModeButtonS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::onRunViewModeButton)
    );
    runViewModeButton->setPosition({-215 + m_size.width / 2, 90 + m_size.height / 2});
    this->m_buttonMenu->addChild(runViewModeButton);

    typeViewModeButtonS = ButtonSprite::create("ReachRate");
    typeViewModeButtonS->setScale(0.4f);
    auto typeViewModeButton = CCMenuItemSpriteExtra::create(
        typeViewModeButtonS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::onTypeViewModeButton)
    );
    typeViewModeButton->setPosition({-215 + m_size.width / 2, 75 + m_size.height / 2});
    typeViewModeButtonS->m_label->setString("PassRate");
    this->m_buttonMenu->addChild(typeViewModeButton);

    m_RunSelectInput = TextInput::create(120, "Run %");
    m_RunSelectInput->getInputNode()->setDelegate(this);
    m_RunSelectInput->setCommonFilter(CommonFilter::Uint);
    m_RunSelectInput->setScale(0.45f);
    m_RunSelectInput->setPosition({-215, 34});
    alignmentNode->addChild(m_RunSelectInput);

    CCArray* runsAllowed = CCArray::create();

    for (int i = 0; i < m_DTLayer->m_MyLevelStats.RunsToSave.size(); i++)
    {
        if (m_DTLayer->m_MyLevelStats.RunsToSave[i] != -1)
            runsAllowed->addObject(ChooseRunCell::create(m_DTLayer->m_MyLevelStats.RunsToSave[i], std::bind(&DTGraphLayer::RunChosen, this, std::placeholders::_1)));
    }
    
    auto runsAllowedView = ListView::create(runsAllowed, 20, 65, 55);

    m_RunsList = GJListLayer::create(runsAllowedView, "", {0,0,0,75}, 65, 55, 1);
    m_RunsList->setPosition({-247, -30});
    alignmentNode->addChild(m_RunsList);

    CCObject* child;

    CCARRAY_FOREACH(m_RunsList->m_listView->m_tableView->m_cellArray, child){
        auto childCell = dynamic_cast<GenericListCell*>(child);
        if (childCell)
            childCell->m_backgroundLayer->setOpacity(30);
    }

    std::vector<CCSprite*> spritesToRemove;
    CCLabelBMFont* title;

    CCARRAY_FOREACH(m_RunsList->getChildren(), child){
        auto childSprite = dynamic_cast<CCSprite*>(child);
        if (childSprite)
            spritesToRemove.push_back(childSprite);
    }

    for (int i = 0; i < spritesToRemove.size(); i++)
    {
        spritesToRemove[i]->removeMeAndCleanup();
    }

    scheduleUpdate();

    return true;
}

void DTGraphLayer::update(float delta){
    if (pointToDisplay.size() == 0){
        npsLabel->setVisible(true);
        PointInfoLabel->setVisible(false);
    }
    else{
        PointInfoLabel->setVisible(true);
        npsLabel->setVisible(false);

        std::string runFixed = pointToDisplay[0]->m_Run;

        if (runFixed != "<" && runFixed.length() > 1){
            //new best color
            if (StatsManager::isKeyInIndex(runFixed, 1, "nbc>")){
                runFixed.erase(0, 5);
            }
            //sessions best color
            if (StatsManager::isKeyInIndex(runFixed, 1, "sbc>")){
                runFixed.erase(0, 5);
            }
        }
        std::string typeText = "Passrate";

        if (currentType == GraphType::ReachRate)
            typeText = "Reachrate";

        PointInfoLabel->setText(fmt::format("Run:\n{}\n \n{}:\n{:.2f}%", runFixed, typeText, pointToDisplay[0]->m_Passrate));
    }
}

void DTGraphLayer::onViewModeButton(CCObject*){
    if (ViewModeNormal){
        ViewModeNormal = false;
        viewModeButtonS->m_label->setString("Session");
    }
    else{
        ViewModeNormal = true;
        viewModeButtonS->m_label->setString("Normal");
    }
    refreshGraph();
}

void DTGraphLayer::onTypeViewModeButton(CCObject*){
    if (currentType == GraphType::PassRate){
        currentType = GraphType::ReachRate;
        typeViewModeButtonS->m_label->setString("ReachRate");
    }
    else if (currentType == GraphType::ReachRate){
        currentType = GraphType::PassRate;
        typeViewModeButtonS->m_label->setString("PassRate");
    }
    refreshGraph();
}

void DTGraphLayer::onRunViewModeButton(CCObject*){
    if (RunViewModeFromZero){
        RunViewModeFromZero = false;
        runViewModeButtonS->m_label->setString("Runs");
    }
    else{
        RunViewModeFromZero = true;
        runViewModeButtonS->m_label->setString("From 0");
    }
    refreshGraph();
}

void DTGraphLayer::textChanged(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_DTLayer->m_SessionsAmount > 0){
        int selected = 1;
        if (!input->getString().empty()){
            auto res = utils::numFromString<int>(input->getString());
            selected = res.unwrapOr(1);
        }

        if (selected > m_DTLayer->m_SessionsAmount){
            selected = m_DTLayer->m_SessionsAmount;
            input->setString(fmt::format("{}", m_DTLayer->m_SessionsAmount));
        }

        if (selected < 1){
            selected = 1;
            input->setString("1");
        }

        m_DTLayer->m_SessionSelectionInput->setString(fmt::format("{}/{}", selected, m_DTLayer->m_SessionsAmount));

        m_DTLayer->m_SessionSelected = selected;
        m_DTLayer->refreshSession();
        if (!ViewModeNormal)
            refreshGraph();
    }

    if (input == m_RunSelectInput->getInputNode()){
        int selected = 0;
        if (!input->getString().empty()){
            auto res = utils::numFromString<int>(input->getString());
            selected = res.unwrapOr(0);
        }

        if (selected > 100){
            selected = 100;
            input->setString("100");
        }

        m_SelectedRunPercent = selected;

        if (!RunViewModeFromZero)
            refreshGraph();
    }
}

void DTGraphLayer::textInputOpened(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_DTLayer->m_SessionsAmount > 0){
        input->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
        m_DTLayer->m_SessionSelectionInputSelected = true;
    }
}

void DTGraphLayer::textInputClosed(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_DTLayer->m_SessionsAmount > 0){
        input->setString(fmt::format("{}/{}", m_DTLayer->m_SessionSelected, m_DTLayer->m_SessionsAmount));
        m_DTLayer->m_SessionSelectionInputSelected = false;
    }
}

/*
    //
    creates a graph with a given deaths string
    change the scaling to change the space between the points on the x and y
    //
*/
CCNode* DTGraphLayer::CreateGraph(
        const std::vector<DeathInfo>& deathsString, const int& bestRun, const ccColor3B& color,
        const CCPoint& Scaling, const ccColor4B& graphBoxOutlineColor, const ccColor4B& graphBoxFillColor, const float& graphBoxOutlineThickness,
        const ccColor4B& labelLineColor, const ccColor4B& labelColor, const int& labelEvery, const ccColor4B& gridColor, const int& gridLineEvery, const GraphType& type
    ){
    if (!deathsString.size()) return nullptr;
    auto toReturnNode = CCNode::create();

    auto LabelsNode = CCNode::create();
    toReturnNode->addChild(LabelsNode);

    CCPoint MaskShape[4] = {
        ccp(0, 0),
        ccp(100 * Scaling.x, 0),
        ccp(100 * Scaling.x, 100 * Scaling.y),
        ccp(0, 100 * Scaling.y)
    };

    auto clippingNode = CCClippingNode::create();
    toReturnNode->addChild(clippingNode);

    auto mask = CCDrawNode::create();
    mask->drawPolygon(MaskShape, 4, ccc4FFromccc4B(graphBoxFillColor), graphBoxOutlineThickness, ccc4FFromccc4B(graphBoxOutlineColor));
    clippingNode->setStencil(mask);
    clippingNode->addChild(mask);

    auto MenuForGP = CCMenu::create();
    MenuForGP->setPosition({0,0});
    MenuForGP->setZOrder(1);
    toReturnNode->addChild(MenuForGP);

    std::vector<CCPoint> lines;

    //log::info("sorting done");

    CCPoint previousPoint = ccp(-1, -1);

    std::map<int, int> ignoredIndexes{};

    std::vector<DeathInfo> fixedDS = deathsString;

    if (type == GraphType::PassRate){
        //add the min and max points if needed
        if (fixedDS[0].run.end > 0){
            auto info = DeathInfo(Run(0, 0), 0, 100);
            fixedDS.insert(fixedDS.begin(), info);
        }
        if (fixedDS[fixedDS.size() - 1].run.end < 100){
            auto info = DeathInfo(Run(0, 100), 0, 0);
            fixedDS.push_back(info);
        }
        else{
            fixedDS[fixedDS.size() - 1].passrate = 100;
        }

        //log::info("added extras");

        for (const auto& deathI : fixedDS)
        {
            //save point
            CCPoint myPoint = ccp(deathI.run.end, deathI.passrate);

            
            //add extra points
            if (previousPoint.x != -1){

                //add a before point if needed
                if (previousPoint.x != myPoint.x - 1){

                    if (previousPoint.x != myPoint.x - 2 && previousPoint.y != 100 && previousPoint.x + 1 <= bestRun){
                        lines.push_back(ccp(previousPoint.x + 1, 100) * Scaling);
                    }

                    if (myPoint.x - 1 <= bestRun && myPoint.y != 100)
                        lines.push_back(ccp(myPoint.x - 1, 100) * Scaling);
                }
            }

            lines.push_back(myPoint * Scaling);
            previousPoint = myPoint;
        }

        //log::info("added lines");

        //add wrapping
        lines.push_back(ccp(lines[lines.size() - 1].x + 100, lines[lines.size() - 1].y));
        lines.push_back(ccp(lines[lines.size() - 1].x + 100, -100));
        lines.push_back(ccp(-100, -100));
        lines.push_back(ccp(-100, lines[0].y));
    }
    else if (type == GraphType::ReachRate){

        int overallCount = 0;
        std::vector<std::pair<int, int>> percentageDeaths{};
        for (int i = fixedDS.size() - 1; i >= 0; i--)
        {
            overallCount += fixedDS[i].deaths;
            percentageDeaths.insert(percentageDeaths.begin(), std::make_pair(fixedDS[i].run.end, overallCount));
        }

        if (percentageDeaths[0].first > 0)
            percentageDeaths.insert(percentageDeaths.begin(), std::make_pair(0, overallCount));
        
        if (percentageDeaths[percentageDeaths.size() - 1].first < 100){
            percentageDeaths.push_back(std::make_pair(percentageDeaths[percentageDeaths.size() - 1].first + 1, 0));

            if (percentageDeaths[percentageDeaths.size() - 1].first != 100)
                percentageDeaths.push_back(std::make_pair(100, 0));
        }

        for (int i = 0; i < percentageDeaths.size(); i++)
        {
            float reachRate = static_cast<float>(percentageDeaths[i].second) / overallCount;

            CCPoint p = ccp(percentageDeaths[i].first * Scaling.x, reachRate * Scaling.y * 100);

            if (p.x != previousPoint.x + 1 * Scaling.x){
                lines.push_back(ccp(previousPoint.x + 1 * Scaling.x, reachRate * Scaling.y * 100));
                ignoredIndexes.insert({static_cast<int>(lines.size()) - 1, static_cast<int>(lines.size()) - 1});
            }

            lines.push_back(p);

            previousPoint = p;
        }
        
    }

    ccColor3B colorOfPoints;

    //add points
    if ((color.r + color.g + color.b) / 3 > 200)
        colorOfPoints = {255, 255, 255};
    else
        colorOfPoints = { 136, 136, 136};

    for (int i = 0; i < lines.size(); i++)
    {
        if (ignoredIndexes.contains(i)) continue;
        if (lines[i].x >= 0 && lines[i].x <= 100 * Scaling.x && lines[i].y >= 0 && lines[i].y <= 100 * Scaling.y)
        {
            auto GP = GraphPoint::create(fmt::format("{}%", lines[i].x / Scaling.x), lines[i].y / Scaling.y, colorOfPoints);
            GP->setDelegate(this);
            GP->setPosition(lines[i]);
            GP->setScale(Settings::getGraphPointSize() / 20 + 0.01f);
            MenuForGP->addChild(GP);
        }
    }

    //create graph
    auto line = CCDrawNode::create();
    for (int i = 0; i < lines.size(); i++)
    {
        if (i != 0){
            if (!line->drawSegment(lines[i - 1], lines[i], 1, ccc4FFromccc3B(color))){
                return CreateGraph(deathsString, bestRun, color, Scaling, graphBoxOutlineColor, graphBoxFillColor, graphBoxOutlineThickness, labelLineColor, labelColor, labelEvery, gridColor, gridLineEvery, currentType);
            }
        }
    }
    clippingNode->addChild(line);

    //create measuring labels
    auto tempT = CCLabelBMFont::create("100", "chatFont.fnt");
    tempT->setScale(0.4f);
    float XForPr = tempT->getScaledContentSize().width;

    auto gridNode = CCDrawNode::create();
    gridNode->setZOrder(-1);
    clippingNode->addChild(gridNode);

    for (int i = 0; i <= 100; i++)
    {
        auto labelPr = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
        labelPr->setPositionX(i * Scaling.x);
        labelPr->setRotation(90);
        labelPr->setColor({labelLineColor.r, labelLineColor.g, labelLineColor.b});
        labelPr->setOpacity(labelLineColor.a);
        LabelsNode->addChild(labelPr);
        
        if (floor(static_cast<float>(i) / labelEvery) == static_cast<float>(i) / labelEvery){
            labelPr->setScaleX(0.2f);

            auto labelPrText = CCLabelBMFont::create(std::to_string(i).c_str(), "chatFont.fnt");
            labelPrText->setPositionX(i * Scaling.x);
            labelPrText->setScale(0.4f);
            labelPrText->setPositionY(-labelPr->getScaledContentSize().width - labelPrText->getScaledContentSize().height);
            labelPrText->setColor({labelColor.r, labelColor.g, labelColor.b});
            labelPrText->setOpacity(labelColor.a);
            LabelsNode->addChild(labelPrText);
        }
        else{
            labelPr->setScaleX(0.1f);
            labelPr->setScaleY(0.8f);
        }
        labelPr->setPositionY(-labelPr->getScaledContentSize().width);
        

        //

        auto labelPS = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
        labelPS->setPositionY(i * Scaling.y);
        labelPS->setColor({labelLineColor.r, labelLineColor.g, labelLineColor.b});
        labelPS->setOpacity(labelLineColor.a);
        LabelsNode->addChild(labelPS);

        if (floor(static_cast<float>(i) / labelEvery) == static_cast<float>(i) / labelEvery){
            labelPS->setScaleX(0.2f);

            auto labelPSText = CCLabelBMFont::create(std::to_string(i).c_str(), "chatFont.fnt");
            labelPSText->setPositionY(i * Scaling.y);
            labelPSText->setScale(0.4f);
            labelPSText->setPositionX(-labelPS->getScaledContentSize().width - XForPr);
            labelPSText->setColor({labelColor.r, labelColor.g, labelColor.b});
            labelPSText->setOpacity(labelColor.a);
            LabelsNode->addChild(labelPSText);
        }
        else{
            labelPS->setScaleX(0.1f);
            labelPS->setScaleY(0.8f);
        }

        labelPS->setPositionX(-labelPS->getScaledContentSize().width);

        //add grid

        if (floor(static_cast<float>(i) / gridLineEvery) == static_cast<float>(i) / gridLineEvery){
            if (
                !gridNode->drawSegment(ccp(0, i * Scaling.y), ccp(100 * Scaling.x, i * Scaling.y), 0.2f, ccc4FFromccc4B(gridColor)) || 
                !gridNode->drawSegment(ccp(i * Scaling.x, 0), ccp(i * Scaling.x, 100 * Scaling.y), 0.2f, ccc4FFromccc4B(gridColor))
            ){
                return CreateGraph(deathsString, bestRun, color, Scaling, graphBoxOutlineColor, graphBoxFillColor, graphBoxOutlineThickness, labelLineColor, labelColor, labelEvery, gridColor, gridLineEvery, currentType);
            }
        }
    }
    

    return toReturnNode;
}

CCNode* DTGraphLayer::CreateRunGraph(
        const std::vector<DeathInfo>& deathsString, const int& bestRun, const ccColor3B& color,
        const CCPoint& Scaling, const ccColor4B& graphBoxOutlineColor, const ccColor4B& graphBoxFillColor, const float& graphBoxOutlineThickness,
        const ccColor4B& labelLineColor, const ccColor4B& labelColor, const int& labelEvery, const ccColor4B& gridColor, const int& gridLineEvery, const GraphType& type
    ){
    if (!deathsString.size()) return nullptr;

    auto toReturnNode = CCNode::create();

    std::vector<DeathInfo> fixedDS = deathsString;

    auto LabelsNode = CCNode::create();
    toReturnNode->addChild(LabelsNode);

    CCPoint MaskShape[4] = {
        ccp(0, 0),
        ccp(100 * Scaling.x, 0),
        ccp(100 * Scaling.x, 100 * Scaling.y),
        ccp(0, 100 * Scaling.y)
    };

    auto clippingNode = CCClippingNode::create();
    toReturnNode->addChild(clippingNode);

    auto mask = CCDrawNode::create();
    mask->drawPolygon(MaskShape, 4, ccc4FFromccc4B(graphBoxFillColor), graphBoxOutlineThickness, ccc4FFromccc4B(graphBoxOutlineColor));
    clippingNode->setStencil(mask);
    clippingNode->addChild(mask);

    auto MenuForGP = CCMenu::create();
    MenuForGP->setPosition({0,0});
    MenuForGP->setZOrder(1);
    toReturnNode->addChild(MenuForGP);

    float RunStartPercent = fixedDS[0].run.start;

    std::vector<CCPoint> lines;

    fixedDS[fixedDS.size() - 1].passrate = 0;

    if (type == GraphType::PassRate){
        //add the min and max points if needed
        if (fixedDS[0].run.end > RunStartPercent){
            auto info = DeathInfo(Run(RunStartPercent, RunStartPercent), 0, 100);
            fixedDS.insert(fixedDS.begin(), info);
        }
        
        if (fixedDS[fixedDS.size() - 1].run.end < 100){
            auto info = DeathInfo(Run(RunStartPercent, 100), 100, 0);
            fixedDS.push_back(info);
        }
        else{
            fixedDS[fixedDS.size() - 1].passrate = 100;
        }
        

        //log::info("added extras");

        CCPoint previousPoint = ccp(-1, -1);

        for (const auto& deathI : fixedDS)
        {
            //save point
            CCPoint myPoint = ccp(deathI.run.end, deathI.passrate);

            
            //add extra points
            if (previousPoint.x != -1){

                //add a before point if needed
                if (previousPoint.x != myPoint.x - 1){

                    if (previousPoint.x != myPoint.x - 2 && previousPoint.y != 100 && previousPoint.x + 1 <= bestRun){
                        lines.push_back(ccp(previousPoint.x + 1, 100) * Scaling);
                    }

                    if (myPoint.x - 1 <= bestRun && myPoint.y != 100)
                        lines.push_back(ccp(myPoint.x - 1, 100) * Scaling);
                }
            }

            lines.push_back(myPoint * Scaling);
            previousPoint = myPoint;
        }

        //add wrapping
        lines.push_back(ccp(lines[lines.size() - 1].x + 100, lines[lines.size() - 1].y));
        lines.push_back(ccp(lines[lines.size() - 1].x + 100, -100));
        lines.push_back(ccp(lines[0].x, -100));
    }
    else if (type == GraphType::ReachRate){

        int overallCount = 0;
        std::vector<std::pair<int, int>> percentageDeaths{};
        for (int i = fixedDS.size() - 1; i >= 0; i--)
        {
            overallCount += fixedDS[i].deaths;
            percentageDeaths.insert(percentageDeaths.begin(), std::make_pair(fixedDS[i].run.end, overallCount));
        }

        if (percentageDeaths[0].first > RunStartPercent)
            percentageDeaths.insert(percentageDeaths.begin(), std::make_pair(RunStartPercent, overallCount));
        
        if (percentageDeaths[percentageDeaths.size() - 1].first < 100){
            percentageDeaths.push_back(std::make_pair(percentageDeaths[percentageDeaths.size() - 1].first + 1, 0));

            if (percentageDeaths[percentageDeaths.size() - 1].first != 100)
                percentageDeaths.push_back(std::make_pair(100, 0));
        }
            

        for (int i = 0; i < percentageDeaths.size(); i++)
        {
            float reachRate = static_cast<float>(percentageDeaths[i].second) / overallCount;
            lines.push_back(ccp(percentageDeaths[i].first * Scaling.x, reachRate * Scaling.y * 100));
        }
        
    }

    //log::info("added lines");

    ccColor3B colorOfPoints;

    if ((color.r + color.g + color.b) / 3 > 200)
        colorOfPoints = {255, 255, 255};
    else
        colorOfPoints = { 136, 136, 136};

    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i].x >= 0 && lines[i].x <= 100 * Scaling.x && lines[i].y >= 0 && lines[i].y <= 100 * Scaling.y)
        {
            auto GP = GraphPoint::create(fmt::format("{}% - {}%", RunStartPercent, lines[i].x / Scaling.x), lines[i].y / Scaling.y, colorOfPoints);
            GP->setDelegate(this);
            GP->setPosition(lines[i]);
            GP->setScale(Settings::getGraphPointSize() / 20 + 0.01f);
            MenuForGP->addChild(GP);
        }
    }

    auto line = CCDrawNode::create();
    for (int i = 0; i < lines.size(); i++)
    {
        if (i != 0){
            if (!line->drawSegment(lines[i - 1], lines[i], 1, ccc4FFromccc3B(color))){
                return CreateRunGraph(deathsString, bestRun, color, Scaling, graphBoxOutlineColor, graphBoxFillColor, graphBoxOutlineThickness, labelLineColor, labelColor, labelEvery, gridColor, gridLineEvery, currentType);
            }
        }
    }
    clippingNode->addChild(line);

    auto tempT = CCLabelBMFont::create("100", "chatFont.fnt");
    tempT->setScale(0.4f);
    float XForPr = tempT->getScaledContentSize().width;

    auto gridNode = CCDrawNode::create();
    gridNode->setZOrder(-1);
    clippingNode->addChild(gridNode);

    for (int i = 0; i <= 100; i++)
    {
        auto labelPr = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
        labelPr->setPositionX(i * Scaling.x);
        labelPr->setRotation(90);
        labelPr->setColor({labelLineColor.r, labelLineColor.g, labelLineColor.b});
        labelPr->setOpacity(labelLineColor.a);
        LabelsNode->addChild(labelPr);
        
        if (floor(static_cast<float>(i) / labelEvery) == static_cast<float>(i) / labelEvery){
            labelPr->setScaleX(0.2f);

            auto labelPrText = CCLabelBMFont::create(std::to_string(i).c_str(), "chatFont.fnt");
            labelPrText->setPositionX(i * Scaling.x);
            labelPrText->setScale(0.4f);
            labelPrText->setPositionY(-labelPr->getScaledContentSize().width - labelPrText->getScaledContentSize().height);
            labelPrText->setColor({labelColor.r, labelColor.g, labelColor.b});
            labelPrText->setOpacity(labelColor.a);
            LabelsNode->addChild(labelPrText);
        }
        else{
            labelPr->setScaleX(0.1f);
            labelPr->setScaleY(0.8f);
        }
        labelPr->setPositionY(-labelPr->getScaledContentSize().width);
        

        //

        auto labelPS = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
        labelPS->setPositionY(i * Scaling.y);
        labelPS->setColor({labelLineColor.r, labelLineColor.g, labelLineColor.b});
        labelPS->setOpacity(labelLineColor.a);
        LabelsNode->addChild(labelPS);

        if (floor(static_cast<float>(i) / labelEvery) == static_cast<float>(i) / labelEvery){
            labelPS->setScaleX(0.2f);

            auto labelPSText = CCLabelBMFont::create(std::to_string(i).c_str(), "chatFont.fnt");
            labelPSText->setPositionY(i * Scaling.y);
            labelPSText->setScale(0.4f);
            labelPSText->setPositionX(-labelPS->getScaledContentSize().width - XForPr);
            labelPSText->setColor({labelColor.r, labelColor.g, labelColor.b});
            labelPSText->setOpacity(labelColor.a);
            LabelsNode->addChild(labelPSText);
        }
        else{
            labelPS->setScaleX(0.1f);
            labelPS->setScaleY(0.8f);
        }

        labelPS->setPositionX(-labelPS->getScaledContentSize().width);

        //grid

        if (floor(static_cast<float>(i) / gridLineEvery) == static_cast<float>(i) / gridLineEvery){
            if (
                !gridNode->drawSegment(ccp(0, i * Scaling.y), ccp(100 * Scaling.x, i * Scaling.y), 0.2f, ccc4FFromccc4B(gridColor)) || 
                !gridNode->drawSegment(ccp(i * Scaling.x, 0), ccp(i * Scaling.x, 100 * Scaling.y), 0.2f, ccc4FFromccc4B(gridColor))
            ){
                return CreateRunGraph(deathsString, bestRun, color, Scaling, graphBoxOutlineColor, graphBoxFillColor, graphBoxOutlineThickness, labelLineColor, labelColor, labelEvery, gridColor, gridLineEvery, currentType);
            }
        }
    }
    

    return toReturnNode;
}

int DTGraphLayer::GetBestRun(const NewBests& bests){
    int bestRun = 0;

    for (auto best : bests)
    {
        if (best > bestRun) bestRun = best;
    }

    return bestRun;
}

int DTGraphLayer::GetBestRunDeathS(const std::vector<DeathInfo>& selectedPercentDeathsInfo){
    int bestRun = 0;

    for (auto best : selectedPercentDeathsInfo)
    {        
        if (best.run.end > bestRun) bestRun = best.run.end;
    }

    return bestRun;
}

int DTGraphLayer::GetBestRun(const std::vector<DeathInfo>& selectedPercentRunInfo){
    int bestRun = 0;

    for (auto best : selectedPercentRunInfo)
    {
        if (best.run.end > bestRun) bestRun = best.run.end;
    }

    return bestRun;
}

void DTGraphLayer::OnPointSelected(cocos2d::CCNode* point){
    pointToDisplay.insert(pointToDisplay.end(), static_cast<GraphPoint*>(point));
}

void DTGraphLayer::OnPointDeselected(cocos2d::CCNode* point){
    for (int i = 0; i < pointToDisplay.size(); i++)
    {
        if (pointToDisplay[i] == point){
            pointToDisplay.erase(std::next(pointToDisplay.begin(), i));
            break;
        }
    }
}

void DTGraphLayer::switchedSessionRight(CCObject*){
    if (m_DTLayer->m_SessionSelected >= m_DTLayer->m_SessionsAmount) return;

    m_DTLayer->m_SessionSelected += 1;
    if (m_DTLayer->m_SessionSelectionInputSelected){
        m_DTLayer->m_SessionSelectionInput->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
        m_SessionSelectionInput->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
    }
    else{
        m_DTLayer->m_SessionSelectionInput->setString(fmt::format("{}/{}",m_DTLayer-> m_SessionSelected, m_DTLayer->m_SessionsAmount));
        m_SessionSelectionInput->setString(fmt::format("{}/{}",m_DTLayer-> m_SessionSelected, m_DTLayer->m_SessionsAmount));
    }
    m_DTLayer->refreshSession();
    if (!ViewModeNormal)
        refreshGraph();
}

void DTGraphLayer::switchedSessionLeft(CCObject*){
    if (m_DTLayer->m_SessionSelected - 1 < 1) return;

    m_DTLayer->m_SessionSelected -= 1;
    if (m_DTLayer->m_SessionSelectionInputSelected){
        m_DTLayer->m_SessionSelectionInput->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
        m_SessionSelectionInput->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
    }
    else{
        m_DTLayer->m_SessionSelectionInput->setString(fmt::format("{}/{}", m_DTLayer->m_SessionSelected, m_DTLayer->m_SessionsAmount));
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_DTLayer->m_SessionSelected, m_DTLayer->m_SessionsAmount));
    }
        
    m_DTLayer->refreshSession();
    if (!ViewModeNormal)
        refreshGraph();
}

void DTGraphLayer::onClose(cocos2d::CCObject*) {
    m_DTLayer->refreshAll();
    m_DTLayer->m_SessionSelectionInputSelected = false;
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void DTGraphLayer::refreshGraph(){
    if (m_graph) m_graph->removeMeAndCleanup();

    if (ViewModeNormal){
        if (RunViewModeFromZero){
            m_graph = CreateGraph(m_DTLayer->m_DeathsInfo, GetBestRunDeathS(m_DTLayer->m_DeathsInfo), Save::getNewBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5, currentType);
        }
        else{
            std::vector<DeathInfo> selectedPercentRunInfo;
            for (int i = 0; i < m_DTLayer->m_RunInfo.size(); i++)
            {

                if (m_DTLayer->m_RunInfo[i].run.start == m_SelectedRunPercent){
                    selectedPercentRunInfo.push_back(m_DTLayer->m_RunInfo[i]);
                }
                        
            }

            m_graph = CreateRunGraph(selectedPercentRunInfo, GetBestRun(selectedPercentRunInfo), Save::getNewBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5, currentType);
        }
        
        if (m_graph){
            m_graph->setPosition({-155, -108});
            m_graph->setZOrder(1);
            alignmentNode->addChild(m_graph);
            noGraphLabel->setVisible(false);
        }
        else{
            noGraphLabel->setVisible(true);
        }
    }
    else{
        if (RunViewModeFromZero){
            m_graph = CreateGraph(m_DTLayer->selectedSessionInfo, GetBestRunDeathS(m_DTLayer->selectedSessionInfo), Save::getSessionBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5, currentType);
        }
        else{
            std::vector<DeathInfo> selectedPercentRunInfo;
            for (int i = 0; i < m_DTLayer->m_SelectedSessionRunInfo.size(); i++)
            {
                if (m_DTLayer->m_SelectedSessionRunInfo[i].run.start == m_SelectedRunPercent){
                    selectedPercentRunInfo.push_back(m_DTLayer->m_SelectedSessionRunInfo[i]);
                }
            }

            m_graph = CreateRunGraph(selectedPercentRunInfo, GetBestRun(selectedPercentRunInfo), Save::getSessionBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5, currentType);
        }

        if (m_graph){
            m_graph->setPosition({-155, -108});
            m_graph->setZOrder(1);
            alignmentNode->addChild(m_graph);
            noGraphLabel->setVisible(false);
        }
        else{
            noGraphLabel->setVisible(true);
        }
    }
    
}

void DTGraphLayer::RunChosen(const int& run){
    m_RunSelectInput->setString(std::to_string(run));
    m_SelectedRunPercent = run;
    if (!RunViewModeFromZero)
        refreshGraph();
}

void DTGraphLayer::onOverallInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "You can change what the graph represents using the buttons under the <cy>\"View Mode\"</c>\nwhen in run mode you choose what run you see using the input field below\n \nYou can click any point on the graph to see its info! the info is displayed on the bottom left.", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}
