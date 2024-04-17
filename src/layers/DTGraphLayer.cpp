#include "../layers/DTGraphLayer.hpp"

DTGraphLayer* DTGraphLayer::create(DTLayer* const& layer) {
    auto ret = new DTGraphLayer();
    if (ret && ret->init(520, 280, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTGraphLayer::setup(DTLayer* const& layer) {

    m_DTLayer = layer;

    this->setOpacity(0);

    noGraphLabel = CCLabelBMFont::create("No Progress\nFor Graph", "bigFont.fnt");
    noGraphLabel->setZOrder(1);
    noGraphLabel->setVisible(false);
    noGraphLabel->setPosition({318, 161});
    this->addChild(noGraphLabel);

    refreshGraph();

    CCScale9Sprite* FontTextDisplayBG = CCScale9Sprite::create("square02b_001.png", {0,0, 80, 80});
    FontTextDisplayBG->setPosition({318, 161});
    FontTextDisplayBG->setContentSize({430, 256});
    FontTextDisplayBG->setColor({0,0,0});
    FontTextDisplayBG->setOpacity(125);
    this->addChild(FontTextDisplayBG);

    CCScale9Sprite* InfoBG = CCScale9Sprite::create("square02b_001.png", {0,0, 80, 80});
    InfoBG->setPosition({69, 75});
    InfoBG->setContentSize({65, 83});
    InfoBG->setColor({ 113, 167, 255 });
    InfoBG->setOpacity(78);
    this->addChild(InfoBG);

    auto InfoLabel = CCLabelBMFont::create("Point Info", "bigFont.fnt");
    InfoLabel->setPosition({69, 124});
    InfoLabel->setScale(0.35f);
    this->addChild(InfoLabel);

    npsLabel = CCLabelBMFont::create("No\nPoint\nSelected", "bigFont.fnt");
    npsLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    npsLabel->setScale(0.35f);
    npsLabel->setPosition({69, 75});
    npsLabel->setZOrder(1);
    npsLabel->setVisible(false);
    npsLabel->setPositionY(npsLabel->getPositionY());
    this->addChild(npsLabel);

    PointInfoLabel = SimpleTextArea::create("Precent\n \nPassrate:\npassrate", "bigFont.fnt");
    PointInfoLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    PointInfoLabel->setPosition({69, 75});
    PointInfoLabel->setZOrder(1);
    PointInfoLabel->setVisible(false);
    PointInfoLabel->setScale(0.35f);
    this->addChild(PointInfoLabel);

    auto SessionSelectCont = CCNode::create();
    SessionSelectCont->setID("Session-Select-Container");
    SessionSelectCont->setPosition({69, 225});
    SessionSelectCont->setScale(0.85f);
    m_mainLayer->addChild(SessionSelectCont);

    auto m_SessionSelectMenu = CCMenu::create();
    m_SessionSelectMenu->setPosition({0, 0});
    SessionSelectCont->addChild(m_SessionSelectMenu);

    std::ranges::sort(m_DTLayer->m_MyLevelStats.sessions, [](const Session a, const Session b) {
        return a.lastPlayed > b.lastPlayed;
    });

    m_SessionSelectionInput = InputNode::create(120, "Session");
    if (m_DTLayer->m_SessionsAmount == 0)
        m_SessionSelectionInput->setString("No sessions.");
    else
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_DTLayer->m_SessionSelected, m_DTLayer->m_SessionsAmount));
    m_SessionSelectionInput->getInput()->setDelegate(this);
    m_SessionSelectionInput->getInput()->setAllowedChars("1234567890");
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
    viewModeLabel->setPosition({69, 275});
    this->addChild(viewModeLabel);

    viewModeButtonS = ButtonSprite::create("Normal");
    viewModeButtonS->setScale(0.475f);
    auto viewModeButton = CCMenuItemSpriteExtra::create(
        viewModeButtonS,
        nullptr,
        this,
        menu_selector(DTGraphLayer::onViewModeButton)
    );
    viewModeButton->setPosition({-215, 99});
    this->m_buttonMenu->addChild(viewModeButton);

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
            if (m_DTLayer->isKeyInIndex(runFixed, 1, "nbc>")){
                runFixed.erase(0, 5);
            }
            //sessions best color
            if (m_DTLayer->isKeyInIndex(runFixed, 1, "sbc>")){
                runFixed.erase(0, 5);
            }
        }

        PointInfoLabel->setText(fmt::format("Run:\n{}\n \nPassrate:\n{:.2f}%", runFixed, pointToDisplay[0]->m_Passrate));
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

void DTGraphLayer::textChanged(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInput() && m_DTLayer->m_SessionsAmount > 0){
        int selected = 1;
        if (input->getString() != "")
            selected = std::stoi(input->getString());
        
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
        m_DTLayer->updateSessionString(m_DTLayer->m_SessionSelected);
        if (!ViewModeNormal)
            refreshGraph();
    }
}

void DTGraphLayer::textInputOpened(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInput() && m_DTLayer->m_SessionsAmount > 0){
        input->setString(fmt::format("{}", m_DTLayer->m_SessionSelected));
        m_DTLayer->m_SessionSelectionInputSelected = true;
    }
}

void DTGraphLayer::textInputClosed(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInput() && m_DTLayer->m_SessionsAmount > 0){
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
CCNode* DTGraphLayer::CreateGraph(std::vector<std::tuple<std::string, int, float>> deathsString, float bestRun, ccColor3B color, CCPoint Scaling, ccColor4B graphBoxOutlineColor, ccColor4B graphBoxFillColor, float graphBoxOutlineThickness, ccColor4B labelLineColor, ccColor4B labelColor, int labelEvery, ccColor4B gridColor, int gridLineEvery){
    if (std::get<0>(deathsString[0]) == "-1" || std::get<0>(deathsString[0]) == "No Saved Progress") return nullptr;

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

    //calculate the points of the graph
    std::vector<std::tuple<CCPoint, std::string, float>> drawPoints;

    for (int i = 0; i < 101; i++)
    {
        bool makePointB = true;
        for (int d = 0; d < deathsString.size(); d++)
        {
            std::string editedDeathString = std::get<0>(deathsString[d]);
            if (StatsManager::ContainsAtIndex(0, "<nbc>", editedDeathString) || StatsManager::ContainsAtIndex(0, "<sbc>", editedDeathString)){
                editedDeathString = editedDeathString.erase(0, 5);
            }

            for (int b = 0; b < editedDeathString.length(); b++)
            {
                if (editedDeathString[b] == '%'){
                    editedDeathString.erase(b, editedDeathString.length() - b);
                    break;
                }
            }
            if (std::stof(editedDeathString) == i){
                if (i == 100){
                    drawPoints.push_back(std::tuple<CCPoint, std::string, float>(ccp(static_cast<float>(i), 100), std::get<0>(deathsString[d]), 100));
                }
                else{
                    drawPoints.push_back(std::tuple<CCPoint, std::string, float>(ccp(static_cast<float>(i), std::get<2>(deathsString[d])), std::get<0>(deathsString[d]), std::get<2>(deathsString[d])));
                }
                
                makePointB = false;
            }
        }

        if (i == 0 && makePointB){
            drawPoints.insert(drawPoints.begin(), std::tuple<CCPoint, std::string, float>(ccp(0, 100), "0%", 100));
        }

        bool IBehind = false;
        bool IInfront = false;
        if (!makePointB){
            if (drawPoints.size() > 1){
                if (std::get<0>(drawPoints[drawPoints.size() - 2]).x != i - 1 && i != 100){
                    IBehind = true;
                }
            }
        }
        else{
            if (drawPoints.size() > 0)
                if (std::get<0>(drawPoints[drawPoints.size() - 1]).y != 100 && i > std::get<0>(drawPoints[drawPoints.size() - 1]).x){
                    if (i > bestRun){
                        drawPoints.insert(drawPoints.end(), std::tuple<CCPoint, std::string, float>(ccp(100, 0), "100%", 0));
                    }
                    else{
                        IInfront = true;
                    }
                }
        }

        if (IBehind)
            drawPoints.insert(drawPoints.end() - 1, std::tuple<CCPoint, std::string, float>(ccp(static_cast<float>(i - 1), 100), std::to_string(i - 1) + "%", 100));

        if (IInfront)
            drawPoints.insert(drawPoints.end(), std::tuple<CCPoint, std::string, float>(ccp(static_cast<float>(i), 100), std::to_string(i) + "%", 100));

    }

    CCPoint pushMinusLine0;

    //connect those points with lines
    for (int i = 0; i < drawPoints.size(); i++)
    {
        if (std::get<0>(drawPoints[i]).x == 0){
            pushMinusLine0 = ccp(-10 * Scaling.x,std::get<0>(drawPoints[i]).y * Scaling.y);
        }

        if (std::get<0>(drawPoints[i]).x == 100){
            lines.push_back({110 * Scaling.x, std::get<0>(drawPoints[i]).y * Scaling.y});
        }
    }
    
    lines.push_back({110 * Scaling.x, -10 * Scaling.y});
    lines.push_back({-10 * Scaling.x, -10 * Scaling.y});
    lines.push_back(pushMinusLine0);

    ccColor3B colorOfPoints;

    if ((color.r + color.g + color.b) / 3 > 200)
        colorOfPoints = {255, 255, 255};
    else
        colorOfPoints = { 136, 136, 136};

    for (int i = 0; i < drawPoints.size(); i++)
    {
        
        auto GP = GraphPoint::create(std::get<1>(drawPoints[i]), std::get<2>(drawPoints[i]), colorOfPoints);
        GP->setDelegate(this);
        CCPoint currPoint = {std::get<0>(drawPoints[i]).x * Scaling.x, std::get<0>(drawPoints[i]).y * Scaling.y};
        GP->setPosition(currPoint);
        GP->setScale(0.05f);
        MenuForGP->addChild(GP);
        lines.push_back(currPoint);

    }

    auto line = CCDrawNode::create();
    line->drawPolygon(&lines[0], lines.size(), ccc4FFromccc4B({ 0, 0, 0, 0}), 1, ccc4FFromccc3B(color));
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
            CCPoint gridLineH[4]{
                ccp(0, i * Scaling.y),
                ccp(0, i * Scaling.y),
                ccp(100 * Scaling.x, i * Scaling.y),
                ccp(100 * Scaling.x, i * Scaling.y)
            };

            CCPoint gridLineS[4]{
                ccp(i * Scaling.x, 0),
                ccp(i * Scaling.x, 0),
                ccp(i * Scaling.x, 100 * Scaling.y),
                ccp(i * Scaling.x, 100 * Scaling.y)
            };

            gridNode->drawPolygon(gridLineH, 4, ccc4FFromccc4B(gridColor), 0.2f, ccc4FFromccc4B(gridColor));
            gridNode->drawPolygon(gridLineS, 4, ccc4FFromccc4B(gridColor), 0.2f, ccc4FFromccc4B(gridColor));
        }
    }
    

    return toReturnNode;
}

float DTGraphLayer::GetBestRun(NewBests bests){
    int bestRun = 0;

    for (auto const& best : bests)
    {
        if (best > bestRun) bestRun = best;
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
    m_DTLayer->updateSessionString(m_DTLayer->m_SessionSelected);
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
        
    m_DTLayer->updateSessionString(m_DTLayer->m_SessionSelected);
    if (!ViewModeNormal)
        refreshGraph();
}

void DTGraphLayer::onClose(cocos2d::CCObject*) {
    m_DTLayer->RefreshText();
    m_DTLayer->m_SessionSelectionInputSelected = false;
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void DTGraphLayer::refreshGraph(){
    if (m_graph) m_graph->removeMeAndCleanup();

    if (ViewModeNormal){
        m_graph = CreateGraph(m_DTLayer->m_DeathsInfo, GetBestRun(m_DTLayer->m_MyLevelStats.newBests), Save::getNewBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5);
        if (m_graph){
            m_graph->setPosition({129, 52});
            m_graph->setZOrder(1);
            this->addChild(m_graph);
            noGraphLabel->setVisible(false);
        }
        else{
            noGraphLabel->setVisible(true);
        }
    }
    else{
        m_graph = CreateGraph(m_DTLayer->selectedSessionInfo, GetBestRun(m_DTLayer->m_MyLevelStats.sessions[m_DTLayer->m_SessionSelected - 1].newBests), Save::getSessionBestColor(), {4, 2.3f}, { 124, 124, 124, 255}, {0, 0, 0, 120}, 0.2f, {115, 115, 115, 255}, { 202, 202, 202, 255}, 5, { 29, 29, 29, 255 }, 5);
        if (m_graph){
            m_graph->setPosition({129, 52});
            m_graph->setZOrder(1);
            this->addChild(m_graph);
            noGraphLabel->setVisible(false);
        }
        else{
            noGraphLabel->setVisible(true);
        }
    }
    
}