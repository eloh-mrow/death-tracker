#include "../layers/DTLayer.hpp"
#include "../managers/DTPopupManager.hpp"
#include "../utils/Settings.hpp"
#include "../layers/LabelLayoutWindow.hpp"
#include "../layers/DTGraphLayer.hpp"
#include "../layers/DTLinkLayer.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include "../hooks/CCControlColourPickerBypass.h"
#include "../hooks/DTColorSelectPopup.hpp"

DTLayer* DTLayer::create(GJGameLevel* const& Level) {
    auto ret = new DTLayer();
    if (ret && ret->initAnchored(368, 280, Level, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTLayer::setup(GJGameLevel* const& level) {
    //create trackerLayer
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_Level = level;

    // ================================== //
    // loading data
    m_MyLevelStats = StatsManager::getLevelStats(m_Level);

    UpdateSharedStats();
    // ================================== //

    /*
     * main page
    */

    this->setID("dt-layer");

    this->setZOrder(100);
    this->m_buttonMenu->setZOrder(1);
    m_buttonMenu->setPosition(m_size);
    m_closeBtn->setPosition(m_closeBtn->getPosition() - m_size / 2);
    m_mainLayer->setZOrder(1);

    alignmentNode = CCNode::create();
    alignmentNode->setPosition(m_size / 2);
    alignmentNode->setZOrder(2);
    m_mainLayer->addChild(alignmentNode);

    auto sideBG = CCScale9Sprite::create("square01_001.png", {0,0, 94, 94});
    sideBG->setContentSize({154, 145});
    sideBG->setPosition(alignmentNode->getPosition() + ccp(233.5f, 10));
    sideBG->setScale(0.65f);
    m_mainLayer->addChild(sideBG);

    //texts bg
    m_TextBG = CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    m_TextBG->setContentSize({520 / 1.6f, m_size.height / 1.15f});
    m_TextBG->setPosition({m_size.width / 2, m_size.height / 2});
    m_TextBG->setOpacity(100);
    m_TextBG->setZOrder(10);
    m_mainLayer->addChild(m_TextBG);

    //create a scroll layer for the text
    m_ScrollLayer = ScrollLayer::create({520 / 1.6f, m_size.height / 1.15f});
    m_ScrollLayer->setPosition(m_TextBG->getPosition() - m_ScrollLayer->getContentSize() / 2);
    m_ScrollLayer->setZOrder(11);
    m_mainLayer->addChild(m_ScrollLayer);

    m_ScrollBar = Scrollbar::create(m_ScrollLayer);
    m_ScrollBar->setScale(0.95f);
    m_ScrollBar->setZOrder(11);
    m_ScrollBar->setPosition({154, 0});
    alignmentNode->addChild(m_ScrollBar);

    auto editLayoutBtnS = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
    auto editLayoutBtnSS = CCSprite::create("layout_button.png"_spr);
    editLayoutBtnSS->setPosition(editLayoutBtnS->getContentSize() / 2);
    editLayoutBtnS->addChild(editLayoutBtnSS);
    editLayoutBtnS->setScale(0.8f);
    m_EditLayoutBtn = CCMenuItemSpriteExtra::create(
        editLayoutBtnS,
        nullptr,
        this,
        menu_selector(DTLayer::onEditLayout)
    );
    m_EditLayoutBtn->setPosition(m_size.width / 2 - 3.0f, m_size.height / 2 - 3.0f);
    this->m_buttonMenu->addChild(m_EditLayoutBtn);

    auto LinkLevelsButtonS = CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png");
    LinkLevelsButtonS->setScale(1.2f);
    LinkLevelsButton = CCMenuItemSpriteExtra::create(
        LinkLevelsButtonS,
        nullptr,
        this,
        menu_selector(DTLayer::OnLinkButtonClicked)
    );
    LinkLevelsButton->setPosition({-m_size.width / 2 + 3.0f, -m_size.height / 2 + 3.0f});
    m_buttonMenu->addChild(LinkLevelsButton);

    auto settingsBS = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsBS->setScale(0.75f);
    settingsButton = CCMenuItemSpriteExtra::create(
        settingsBS,
        nullptr,
        this,
        menu_selector(DTLayer::onSettings)
    );
    settingsButton->setPosition({m_size.width / 2 - 8.5f, -m_size.height / 2 + 8.5f});
    this->m_buttonMenu->addChild(settingsButton);

    auto levelSettingsBS = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
    levelSettingsBS->setScale(0.4f);

    levelSettingsBSArrow = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    levelSettingsBSArrow->setPosition({-15, levelSettingsBS->getContentHeight() / 2});
    levelSettingsBSArrow->setScale(0.75f);
    levelSettingsBS->addChild(levelSettingsBSArrow);

    auto levelSettingsButton = CCMenuItemSpriteExtra::create(
        levelSettingsBS,
        nullptr,
        this,
        menu_selector(DTLayer::onSpecificSettings)
    );
    levelSettingsButton->setPosition({-m_size.width / 2 + 3.0f, 0});
    this->m_buttonMenu->addChild(levelSettingsButton);

    /*
     * edit layout
    */

    m_EditLayoutMenu = CCMenu::create();
    m_EditLayoutMenu->setVisible(false);
    m_EditLayoutMenu->setZOrder(12);
    m_EditLayoutMenu->setPosition(m_buttonMenu->getPosition() / 2);
    m_mainLayer->addChild(m_EditLayoutMenu);

    m_BlackSquare = CCSprite::create("square02_001.png");
    m_BlackSquare->setZOrder(9);
    m_BlackSquare->setVisible(false);
    m_BlackSquare->setPosition(winSize / 2);
    m_BlackSquare->setScale(100);
    m_BlackSquare->setOpacity(150);
    alignmentNode->addChild(m_BlackSquare);

    auto editLayoutApplyBtnS = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
    auto editLayoutApplyBtnSS = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    editLayoutApplyBtnSS->setPosition(editLayoutBtnS->getContentSize() / 2);
    editLayoutApplyBtnS->addChild(editLayoutApplyBtnSS);
    editLayoutApplyBtnS->setScale(0.65f);
    editLayoutApplyBtn = CCMenuItemSpriteExtra::create(
        editLayoutApplyBtnS,
        nullptr,
        this,
        menu_selector(DTLayer::onEditLayoutApply)
    );
    editLayoutApplyBtn->setPosition({159, -120});
    m_EditLayoutMenu->addChild(editLayoutApplyBtn);

    auto addWindowButtonS = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    addWindowButtonS->setScale(0.6f);
    addWindowButton = CCMenuItemSpriteExtra::create(
        addWindowButtonS,
        nullptr,
        this,
        menu_selector(DTLayer::addBox)
    );
    addWindowButton->setPosition({-157, 114});
    m_EditLayoutMenu->addChild(addWindowButton);

    auto layoutInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    layoutInfoBS->setScale(0.8f);
    layoutInfoButton = CCMenuItemSpriteExtra::create(
        layoutInfoBS,
        nullptr,
        this,
        menu_selector(DTLayer::onLayoutInfo)
    );
    layoutInfoButton->setPosition({159, 118});
    m_EditLayoutMenu->addChild(layoutInfoButton);

    auto resetLayoutBS = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    resetLayoutBS->setScale(0.5f);
    resetLayoutButton = CCMenuItemSpriteExtra::create(
        resetLayoutBS,
        nullptr,
        this,
        menu_selector(DTLayer::onResetLayout)
    );
    resetLayoutButton->setPosition({-157, -112});
    m_EditLayoutMenu->addChild(resetLayoutButton);

    auto editLayoutSideBG = CCScale9Sprite::create("square01_001.png", {0,0, 94, 94});
    editLayoutSideBG->setContentSize({154, 145});
    editLayoutSideBG->setPosition(ccp(233.5f, 10));
    editLayoutSideBG->setScale(0.65f);
    m_EditLayoutMenu->addChild(editLayoutSideBG);

    colorSpritenb = ColorChannelSprite::create();
    colorSpritenb->setColor(Save::getNewBestColor());
    colorSpritenb->setScale(0.65f);
    auto nbcPicker = CCMenuItemSpriteExtra::create(
        colorSpritenb,
        nullptr,
        this,
        menu_selector(DTLayer::editnbcColor)
    );
    nbcPicker->setPosition({233, 38});
    m_EditLayoutMenu->addChild(nbcPicker);

    colorSpritesb = ColorChannelSprite::create();
    colorSpritesb->setColor(Save::getSessionBestColor());
    colorSpritesb->setScale(0.65f);
    auto sbcPicker = CCMenuItemSpriteExtra::create(
        colorSpritesb,
        nullptr,
        this,
        menu_selector(DTLayer::editsbcColor)
    );
    sbcPicker->setPosition({233, -3});
    m_EditLayoutMenu->addChild(sbcPicker);

    auto nbcColorPickerLabel = CCLabelBMFont::create("New best", "bigFont.fnt");
    nbcColorPickerLabel->setPosition({233, 22});
    nbcColorPickerLabel->setScale(0.4f);
    nbcColorPickerLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_EditLayoutMenu->addChild(nbcColorPickerLabel);

    auto sbcColorPickerLabel = CCLabelBMFont::create("Session best", "bigFont.fnt");
    sbcColorPickerLabel->setPosition({233, -19});
    sbcColorPickerLabel->setScale(0.35f);
    sbcColorPickerLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_EditLayoutMenu->addChild(sbcColorPickerLabel);

    //session selection

    auto SessionSelectCont = CCNode::create();
    SessionSelectCont->setID("Session-Select-Container");
    SessionSelectCont->setPosition({233, 24});
    SessionSelectCont->setZOrder(1);
    alignmentNode->addChild(SessionSelectCont);

    m_SessionSelectMenu = CCMenu::create();
    m_SessionSelectMenu->setPosition({0, 0});
    SessionSelectCont->addChild(m_SessionSelectMenu);

    m_SessionsAmount = m_SharedLevelStats.sessions.size();
    m_SessionSelected = 1;

    m_SessionSelectionInput = TextInput::create(120, "Session");
    if (m_SessionsAmount == 0)
        m_SessionSelectionInput->setString("No sessions.");
    else
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_SessionSelected, m_SessionsAmount));

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
        menu_selector(DTLayer::SwitchSessionRight)
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
        menu_selector(DTLayer::SwitchSessionLeft)
    );
    SessionSelectionLeft->setPosition({-34, 0});
    SessionSelectionLeft->setRotation(180);
    m_SessionSelectMenu->addChild(SessionSelectionLeft);

    auto SessionSelectionLabel = CCLabelBMFont::create("Session", "bigFont.fnt");
    SessionSelectionLabel->setPosition({0, 16});
    SessionSelectionLabel->setScale(0.45f);
    SessionSelectCont->addChild(SessionSelectionLabel);

    auto GraphButtonS = CCSprite::create("graph_button.png"_spr);
    GraphButtonS->setScale(0.85f);
    auto GraphButton = CCMenuItemSpriteExtra::create(
        GraphButtonS,
        nullptr,
        this,
        menu_selector(DTLayer::openGraphMenu)
    );
    GraphButton->setPosition({233, -9});
    m_buttonMenu->addChild(GraphButton);

    //copy text

    auto copyTextBS = ButtonSprite::create("Copy Text");
    copyTextBS->setScale(0.575f);
    copyTextButton = CCMenuItemSpriteExtra::create(
        copyTextBS,
        nullptr,
        this,
        menu_selector(DTLayer::copyText)
    );
    copyTextButton->setPosition({0, -m_size.height / 2 + 3.0f});
    this->m_buttonMenu->addChild(copyTextButton);

    auto copyInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    copyInfoBS->setScale(0.8f);
    copyInfoButton = CCMenuItemSpriteExtra::create(
        copyInfoBS,
        nullptr,
        this,
        menu_selector(DTLayer::onCopyInfo)
    );
    copyInfoButton->setPosition({159, 118});
    copyInfoButton->setVisible(false);
    m_EditLayoutMenu->addChild(copyInfoButton);

    createLayoutBlocks();
    refreshStrings();
    RefreshText(true);

    scheduleUpdate();

    if (Save::getLastOpenedVersion() != Mod::get()->getVersion().toNonVString()){
        Save::setLastOpenedVersion(Mod::get()->getVersion().toNonVString());
        FLAlertLayer::create(nullptr, fmt::format("Death Tracker {} Changelog", Mod::get()->getVersion().toVString()).c_str(), fmt::format(
            "{}",
            "- <cg>fixed a crash when opening the link layer (i hope)</c>"
        ), "OK", nullptr, 415, false, 200, 1)->show();
    }

    return true;
}

void DTLayer::update(float delta){
    m_LayoutStuffCont->setVisible(m_EditLayoutMenu->isVisible());
    m_TextCont->setVisible(!m_EditLayoutMenu->isVisible());
}

void DTLayer::onEditLayout(CCObject* sender){
    EditLayoutEnabled(true);
}

void DTLayer::textChanged(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_SessionsAmount > 0){
        int selected = 1;
        if (input->getString() != ""){
            auto res = utils::numFromString<int>(input->getString());
            selected = res.unwrapOr(1);
        }
        
        if (selected > m_SessionsAmount){
            selected = m_SessionsAmount;
            input->setString(fmt::format("{}", m_SessionsAmount));
        }

        if (selected < 1){
            selected = 1;
            input->setString("1");
        }

        m_SessionSelected = selected;
        updateSessionString(m_SessionSelected);
        RefreshText();
    }
}

void DTLayer::textInputOpened(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_SessionsAmount > 0){
        input->setString(fmt::format("{}", m_SessionSelected));
        m_SessionSelectionInputSelected = true;
    }

    
}

void DTLayer::textInputClosed(CCTextInputNode* input){
    if (input == m_SessionSelectionInput->getInputNode() && m_SessionsAmount > 0){
        input->setString(fmt::format("{}/{}", m_SessionSelected, m_SessionsAmount));
        m_SessionSelectionInputSelected = false;
    }
}

void DTLayer::updateSessionString(int session){
    if (session - 1 < 0 || session - 1 >= m_SharedLevelStats.sessions.size()) return;

    Session currentSession = m_SharedLevelStats.sessions[session - 1];

    auto selectedSessionInfoRes = CreateDeathsString(currentSession.deaths, currentSession.newBests);

    if (!selectedSessionInfoRes.isOk()) return;

    selectedSessionInfo = selectedSessionInfoRes.unwrap();

    std::string mergedString = "";
    for (const auto& SDeathI : selectedSessionInfo)
    {
        if (SDeathI.isNewBest)
            mergedString += "<sbc>";
        mergedString += fmt::format("{}% x{}\n", SDeathI.run.end, SDeathI.deaths);
    }
    mergedString = mergedString.substr(0, mergedString.size() - 1);
    selectedSessionString = mergedString;

    auto SelectedSessionRunInfoRes = CreateRunsString(currentSession.runs);

    if (!SelectedSessionRunInfoRes.isOk()) return;

    m_SelectedSessionRunInfo = SelectedSessionRunInfoRes.unwrap();

    mergedString = "";
    for (const auto& SRunI : m_SelectedSessionRunInfo)
    {
        mergedString += fmt::format("{}% - {}% x{}\n", SRunI.run.start, SRunI.run.end, SRunI.deaths);
    }
    mergedString = mergedString.substr(0, mergedString.size() - 1);
    selectedSessionRunString = mergedString;
}

void DTLayer::onEditLayoutApply(CCObject*){
    EditLayoutEnabled(false);

    std::vector<LabelLayout> layout;
    for (int i = 0; i < m_LayoutLines.size(); i++)
    {
        auto IWindow = static_cast<LabelLayoutWindow*>(m_LayoutLines[i]);
        layout.push_back(IWindow->m_MyLayout);
    }

    Save::setNewBestColor(colorSpritenb->getColor());
    Save::setSessionBestColor(colorSpritesb->getColor());

    Save::setLayout(layout);

    RefreshText(true);
}

bool DTLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    m_IsClicking = true;
    if (pTouch->getLocation() != ccp(0, CCDirector::sharedDirector()->getWinSize().height))
        ClickPos = pTouch;
    return true;
}

void DTLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
    //m_IsClicking = false;
    if (pTouch->getLocation() != ccp(0, CCDirector::sharedDirector()->getWinSize().height))
        ClickPos = pTouch;
}

void DTLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
    m_IsClicking = false;
    //ClickPos = pTouch;
}

void DTLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){
    m_IsClicking = false;
    //ClickPos = pTouch;
}

void DTLayer::EditLayoutEnabled(bool b){
    this->m_buttonMenu->setEnabled(!b);
    m_SessionSelectMenu->setEnabled(!b);
    //m_RunStuffMenu->setEnabled(!b);
    //modifyRunsMenu->setEnabled(!b);
    m_EditLayoutMenu->setVisible(b);
    m_EditLayoutBtn->setEnabled(!b);
    m_BlackSquare->setVisible(b);
    m_LayoutStuffCont->setVisible(b);
    editLayoutApplyBtn->setEnabled(b);
    //addFZRunInput->setEnabled(!b);
    //addRunStartInput->setEnabled(!b);
    //addRunEndInput->setEnabled(!b);
    //runsAmountInput->setEnabled(!b);
    layoutInfoButton->setVisible(b);
    addWindowButton->setVisible(b);
    resetLayoutButton->setVisible(b);
    //nbcColorPickerLabel->setVisible(b);
    //sbcColorPickerLabel->setVisible(b);
    if (m_SessionSelectionInput)
        m_SessionSelectionInput->setEnabled(!b);
    //m_AddRunAllowedInput->setEnabled(!b);

    for (int i = 0; i < m_LayoutLines.size(); i++)
    {
        auto IWindow = static_cast<LabelLayoutWindow*>(m_LayoutLines[i]);
        IWindow->setMoveEnabled(b);
    }

    float transitionFadeSpeed = 0.5f;

    if (b)
    {
        changeScrollSizeByBoxes(true);
        m_TextBG->setOpacity(200);
        //sbcColorPicker->setPosition({-205, -64});
        //nbcColorPicker->setPosition({-205, 33});
        colorSpritenb->setColor(Save::getNewBestColor());
        colorSpritesb->setColor(Save::getSessionBestColor());
    }
    else{
        RefreshText(true);
        //sbcColorPicker->setPosition({-500, -500});
        //nbcColorPicker->setPosition({-500, -500});
        m_TextBG->setOpacity(100);

        auto sprite = static_cast<CCSprite*>(editLayoutApplyBtn->getChildren()->objectAtIndex(0));
        sprite->setOpacity(255);
        static_cast<CCSprite*>(sprite->getChildren()->objectAtIndex(0))->setOpacity(255);
        isInCopyMenu = false;
        //copyInfoButton->setVisible(false);
    }
}

void DTLayer::changeScrollSizeByBoxes(bool moveToTop){
    float hight = 0;
    float lineH = 0;
    LabelLayoutWindow* PrevLine = nullptr;

    std::vector<LabelLayoutWindow*> sortedLayoutLines;
    sortedLayoutLines.reserve(m_LayoutLines.size());
    for (int i = 0; i < m_LayoutLines.size(); i++)
        sortedLayoutLines.push_back(static_cast<LabelLayoutWindow*>(m_LayoutLines[i]));
    
    std::ranges::sort(sortedLayoutLines, [](const LabelLayoutWindow* a, const LabelLayoutWindow* b) {
        return a->m_MyLayout.line < b->m_MyLayout.line;
    });

    for (int i = 0; i < sortedLayoutLines.size(); i++)
    {
        if (PrevLine){
            float calH = abs(sortedLayoutLines[i]->getPositionY() - PrevLine->getPositionY());
            hight += calH;
        }
        else{
            lineH = m_LayoutLines[i]->getContentSize().height;
        }

        PrevLine = sortedLayoutLines[i];
    }
    hight += lineH * 2;

    if (hight < m_ScrollLayer->getContentSize().height){
        m_ScrollLayer->m_contentLayer->setContentSize({m_ScrollLayer->m_contentLayer->getContentSize().width, m_ScrollLayer->getContentSize().height});
    }
    else{
        m_ScrollLayer->m_contentLayer->setContentSize({m_ScrollLayer->m_contentLayer->getContentSize().width, hight});
    }
    
    m_LayoutStuffCont->setPositionX(m_ScrollLayer->getContentSize().width / 2);
    m_LayoutStuffCont->setPositionY(m_ScrollLayer->m_contentLayer->getContentSize().height);

    if (moveToTop)
        m_ScrollLayer->moveToTop();
}

void DTLayer::createLayoutBlocks(){
    auto m_CurretLayout = Save::getLayout();

    if (m_LayoutStuffCont)
        m_LayoutStuffCont->removeMeAndCleanup();

    m_LayoutStuffCont = CCNode::create();
    m_LayoutStuffCont->setPositionX(m_ScrollLayer->getContentSize().width / 2);
    m_LayoutStuffCont->setPositionY(m_ScrollLayer->m_contentLayer->getContentSize().height);
    m_LayoutStuffCont->setVisible(false);

    m_ScrollLayer->m_contentLayer->addChild(m_LayoutStuffCont);

    m_LayoutLines.clear();
    m_LayoutLines.reserve(m_CurretLayout.size());

    for (int i = 0; i < m_CurretLayout.size(); i++)
    {
        auto currentWindow = LabelLayoutWindow::create(m_CurretLayout[i], this);
        m_LayoutStuffCont->addChild(currentWindow);
        handleTouchPriority(this);
        m_LayoutLines.push_back(currentWindow);
    }

    for (int i = 0; i < m_LayoutLines.size(); i++)
    {
        static_cast<LabelLayoutWindow*>(m_LayoutLines[i])->setPositionBasedOnLayout(static_cast<LabelLayoutWindow*>(m_LayoutLines[i])->m_MyLayout);
    }
}

void DTLayer::RefreshText(bool moveToTop){

    if (m_TextCont) m_TextCont->removeMeAndCleanup();

    m_TextCont = CCNode::create();
    m_ScrollLayer->m_contentLayer->addChild(m_TextCont);

    float overallHight = 0;

    auto layout = Save::getLayout();

    std::vector<std::tuple<SimpleTextArea*, int, int>> lables;

    std::map<int, float> positioning;
    std::map<int, bool> twoLabelLines;

    for (const auto& labelSettings : layout)
    {
        auto modifiedString = modifyString(labelSettings.text);

        auto label = SimpleTextArea::create(modifiedString.c_str(), StatsManager::getFont(labelSettings.font).c_str());
        label->setAlignment(labelSettings.alignment);
        label->setAnchorPoint({0.5f, 1});
        label->setWrappingMode(WrappingMode::WORD_WRAP);
        label->setColor(labelSettings.color);
        label->setScale(labelSettings.fontSize);
        lables.push_back(std::make_tuple(label, labelSettings.line, labelSettings.position));
        m_TextCont->addChild(label);

        float contentSize = 0;

        for (const auto& line : label->getLines())
        {
            contentSize += line->getScaledContentSize().height + label->getLinePadding();
        }

        if (positioning.contains(labelSettings.line)){
            twoLabelLines.insert(std::make_pair(labelSettings.line, true));
            if (positioning[labelSettings.line] < contentSize){
                positioning[labelSettings.line] = contentSize;
            }
        }
        else
            positioning.insert(std::make_pair(labelSettings.line, contentSize));
    }

    for (int i = 0; i < lables.size(); i++)
    {
        if (twoLabelLines[std::get<1>(lables[i])]){
            SimpleTextArea* currentArea = std::get<0>(lables[i]);

            currentArea->setWidth({currentArea->getWidth() / 2});
            if (std::get<2>(lables[i]) == 1){
                currentArea->setPositionX(currentArea->getPositionX() + currentArea->getWidth() / 2);
            }
            else{
                currentArea->setPositionX(currentArea->getPositionX() - currentArea->getWidth() / 2);
            }
            currentArea->setFont(currentArea->getFont());
        }
        for (const auto& posI : positioning)
        {
            if (std::get<1>(lables[i]) > posI.first){
                std::get<0>(lables[i])->setPositionY(std::get<0>(lables[i])->getPositionY() - posI.second);
            }
            else{
                break;
            }
        }

        auto label = std::get<0>(lables[i]);
        for (const auto& line : label->getLines())
        {
            std::string s = line->getString();
            if (s != "<" && s.length() > 1){
                //new best color
                if (isKeyInIndex(s, 1, "nbc>")){
                    s.erase(0, 5);
                    line->setString(s.c_str());
                    line->setColor(Save::getNewBestColor());
                }
                //sessions best color
                if (isKeyInIndex(s, 1, "sbc>")){
                    s.erase(0, 5);
                    line->setString(s.c_str());
                    line->setColor(Save::getSessionBestColor()); 
                }
            }

            s = line->getString();

            if (m_Level->isPlatformer()){
                for (int i = 0; i < s.length(); i++)
                {
                    if (s[i] == '%'){
                        auto PS = dynamic_cast<CCNode*>(line->getChildren()->objectAtIndex(i));
                        if (!PS) break;
                        PS->setVisible(false);
                        auto PSCont = CCNode::create();
                        PSCont->setPosition(line->getPosition());
                        PSCont->setScale(line->getScale());
                        PSCont->setAnchorPoint(line->getAnchorPoint());
                        PSCont->setContentSize(line->getContentSize());
                        line->getParent()->addChild(PSCont);

                        auto cpS = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
                        cpS->setPosition(PS->getPosition());
                        PSCont->addChild(cpS);
                    }
                }
                
            }
        }
    }

    for (const auto& posI : positioning)
    {
        overallHight += posI.second;
    }

    if (overallHight < m_ScrollLayer->getContentSize().height){
        m_ScrollLayer->m_contentLayer->setContentSize({m_ScrollLayer->m_contentLayer->getContentSize().width, m_ScrollLayer->getContentSize().height});
    }
    else{
        m_ScrollLayer->m_contentLayer->setContentSize({m_ScrollLayer->m_contentLayer->getContentSize().width, overallHight});
    }

    m_TextCont->setPositionX(m_ScrollLayer->getContentSize().width / 2);
    m_TextCont->setPositionY(m_ScrollLayer->m_contentLayer->getContentSize().height);

    if (moveToTop)
        m_ScrollLayer->moveToTop();
    /*
    create labels by settings

    know how high every line
        get the highest if the two are in the same line
    
    place the labels at the bottom of the previewse line postions added up
    
    place the pos 1 on the right and pos 0 on the left
    */
}

std::string DTLayer::modifyString(std::string ToModify){
    /*
    keys to check for

    {f0} - runs from 0

    {runs} - runs

    {lvln} - levels name
    
    {att} - level attempts (with linked levels attempts)

    {s0} - selected session runs from 0
    
    {sruns} - selected session runs

    {nl} - new line

    {ssd}

    {sst}
    
    */

    int overallOffset = 0;

    auto inctences = StatsManager::KMPSearch("{", ToModify);
    for (int i = 0; i < inctences.size(); i++)
    {
        if (isKeyInIndex(ToModify, inctences[i] + 1, "f0}")){
            ToModify.erase(inctences[i], 4);
            ToModify.insert(inctences[i], deathsString);
            overallOffset -= 4;
            overallOffset += deathsString.length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "runs}")){
            ToModify.erase(inctences[i], 6);
            ToModify.insert(inctences[i], RunString);
            overallOffset -= 6;
            overallOffset += RunString.length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "lvln}")){
            ToModify.erase(inctences[i], 6);
            ToModify.insert(inctences[i], m_SharedLevelStats.levelName);
            overallOffset -= 6;
            overallOffset += m_SharedLevelStats.levelName.length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "att}")){
            ToModify.erase(inctences[i], 5);
            ToModify.insert(inctences[i], std::to_string(m_SharedLevelStats.attempts));
            overallOffset -= 5;
            overallOffset += std::to_string(m_SharedLevelStats.attempts).length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "s0}")){
            ToModify.erase(inctences[i], 4);
            ToModify.insert(inctences[i], selectedSessionString);
            overallOffset -= 4;
            overallOffset += selectedSessionString.length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "sruns}")){
            ToModify.erase(inctences[i], 7);
            ToModify.insert(inctences[i], selectedSessionRunString);
            overallOffset -= 7;
            overallOffset += selectedSessionRunString.length();
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "nl}")){
            ToModify.erase(inctences[i], 4);
            ToModify.insert(inctences[i], "\n");
            overallOffset -= 4;
            overallOffset += 1;
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "ssd}")){
            ToModify.erase(inctences[i], 5);
            overallOffset -= 5;

            if (m_SharedLevelStats.sessions[m_SessionSelected - 1].sessionStartDate == -1){
                std::string toAdd = "(Unknown date)";
                ToModify.insert(inctences[i], toAdd);
                overallOffset += toAdd.length();
            }
            else{
                time_t time = m_SharedLevelStats.sessions[m_SessionSelected - 1].sessionStartDate;
                auto tm = std::localtime(&time);

                ToModify.insert(inctences[i], fmt::format("{}/{}/{}", tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900));
                overallOffset += fmt::format("{}/{}/{}", tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900).length();
            }
        }
        if (isKeyInIndex(ToModify, inctences[i] + 1, "sst}")){
            ToModify.erase(inctences[i], 5);
            overallOffset -= 5;

            if (!m_SharedLevelStats.sessions.size()) continue;

            if (m_SharedLevelStats.sessions[m_SessionSelected - 1].sessionStartDate == -1){
                std::string toAdd = "(Unknown time)";
                ToModify.insert(inctences[i], toAdd);
                overallOffset += toAdd.length();
            }
            else{
                time_t time = m_SharedLevelStats.sessions[m_SessionSelected - 1].sessionStartDate;
                auto tm = std::localtime(&time);

                std::string clock12Time = "AM";

                if (tm->tm_hour > 12){
                    tm->tm_hour -= 12;
                    clock12Time = "PM";
                }

                ToModify.insert(inctences[i], fmt::format("{}:{}{}", tm->tm_hour, tm->tm_min, clock12Time));
                overallOffset += fmt::format("{}:{}{}", tm->tm_hour, tm->tm_min, clock12Time).length();
            }
        }

        if (i + 1 < inctences.size()){
            inctences[i + 1] += overallOffset;
        }
    }
    
    return ToModify;
}

bool DTLayer::isKeyInIndex(const std::string& s, int index, const std::string& key) {
    if (index + key.length() > s.length()) return false;

    return s.substr(index, key.length()) == key;
}

void DTLayer::refreshStrings(){
    auto DeathsInfoRes = CreateDeathsString(m_SharedLevelStats.deaths, m_SharedLevelStats.newBests);

    if (!DeathsInfoRes.isOk()) return;

    m_DeathsInfo = DeathsInfoRes.unwrap();

    std::string mergedString = "";
    for (const auto& deathI : m_DeathsInfo)
    {
        if (deathI.isNewBest)
            mergedString += "<nbc>";
        mergedString += fmt::format("{}% x{}\n", deathI.run.end, deathI.deaths);
    }
    mergedString = mergedString.substr(0, mergedString.size() - 1);
    deathsString = mergedString;

    auto RunInfoRes = CreateRunsString(m_SharedLevelStats.runs);

    if (!RunInfoRes.isOk()) return;

    m_RunInfo = RunInfoRes.unwrap();

    mergedString = "";
    for (const auto& runI : m_RunInfo)
    {
        mergedString += fmt::format("{}% - {}% x{}\n", runI.run.start, runI.run.end, runI.deaths);
    }
    mergedString = mergedString.substr(0, mergedString.size() - 1);
    RunString = mergedString;

    updateSessionString(m_SessionSelected);
}

Result<std::vector<DeathInfo>> DTLayer::CreateDeathsString(const Deaths& deaths, const NewBests& newBests){
    if (!m_Level) return Err("invalid level");
    if (deaths.size() == 0) return Err("No Saved Progress");

    int totalDeaths = 0;
    int bestRun = 0;
    std::map<int, int> sortedDeaths{};

    // sort the deaths
    for (const auto& [percentKey, count] : deaths) {
        GEODE_UNWRAP_INTO(auto precentInt, geode::utils::numFromString<int>(percentKey));

        if (precentInt < m_MyLevelStats.hideUpto) continue;

        sortedDeaths.insert(std::make_pair(precentInt, count));
        totalDeaths += count;
        if (precentInt > bestRun) bestRun = precentInt;
    }

    // create output
    int offset = m_Level->m_normalPercent.value() == 100
        ? 1
        : 0;

    std::vector<DeathInfo> output{};

    for (const auto& [percent, count] : sortedDeaths) {
        // calculate pass rate
        totalDeaths -= count;

        float passCount = totalDeaths;
        float passRate = (passCount + offset) / (passCount + count + offset) * 100;

        bool nb = newBests.contains(percent);
            
        if (percent == bestRun)
            if (bestRun != 100)
                passRate = 0;
            else
                passRate = 100;

        auto info = DeathInfo(Run(0, percent), nb, count, passRate);
        output.push_back(info);
    }

    if (output.size() == 0) return Err("No Saved Progress");
    
    std::ranges::sort(output, [](const DeathInfo a, const DeathInfo b) {
        return a.run.end < b.run.end;
    });

    return Ok(output);
}

Result<std::vector<DeathInfo>> DTLayer::CreateRunsString(const Runs& runs){
    if (!m_Level) return Err("invalid level");
    if (runs.size() == 0) return Err("No Saved Progress");

    std::map<int, int> totalDeaths{};
    std::vector<std::tuple<std::string, int>> sortedRuns{};

    for (const auto [runKey, count] : runs){
        sortedRuns.push_back(std::make_tuple(runKey, count));
                        
        totalDeaths[StatsManager::splitRunKey(runKey).start] += count;
    }

    // sort the runs
    std::ranges::sort(sortedRuns, [](const std::tuple<std::string, int> a, const std::tuple<std::string, int> b) {
        auto runA = StatsManager::splitRunKey(std::get<0>(a));
        auto runB = StatsManager::splitRunKey(std::get<0>(b));

        // start is equal, compare end
        if (runA.start == runB.start) return runA.end < runB.end;
        return runA.start < runB.start;
    });

    // create output
    std::vector<DeathInfo> output{};

    for (const auto [runKey, count] : sortedRuns) {

        auto splittedRun = StatsManager::splitRunKey(runKey); 

        totalDeaths[splittedRun.start] -= count;

        float passCount = totalDeaths[splittedRun.start];
        float passRate = (passCount) / (passCount + count) * 100;

        auto info = DeathInfo(splittedRun, false, count, passRate);

        output.push_back(info);
    }

    return Ok(output);
}

void DTLayer::SwitchSessionRight(CCObject*){
    if (m_SessionSelected >= m_SessionsAmount) return;

    m_SessionSelected += 1;
    if (m_SessionSelectionInputSelected)
        m_SessionSelectionInput->setString(fmt::format("{}", m_SessionSelected));
    else
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_SessionSelected, m_SessionsAmount));
    updateSessionString(m_SessionSelected);
    RefreshText();
}   

void DTLayer::SwitchSessionLeft(CCObject*){
    if (m_SessionSelected - 1 < 1) return;

    m_SessionSelected -= 1;
    if (m_SessionSelectionInputSelected)
        m_SessionSelectionInput->setString(fmt::format("{}", m_SessionSelected));
    else
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_SessionSelected, m_SessionsAmount));
    updateSessionString(m_SessionSelected);
    RefreshText();
}

void DTLayer::addBox(CCObject*){
    int lastLine = 0;

    for (int i = 0; i < m_LayoutLines.size(); i++)
    {
        if (static_cast<LabelLayoutWindow*>(m_LayoutLines[i])->m_MyLayout.line > lastLine){
            lastLine = static_cast<LabelLayoutWindow*>(m_LayoutLines[i])->m_MyLayout.line;
        }
    }
    
    LabelLayout currentLayout
    {
        .labelName = "Label",
        .text = "",
        .line = lastLine + 1,
        .position = 0,
        .color = {255, 255, 255, 255},
        .alignment = CCTextAlignment::kCCTextAlignmentCenter,
        .font = 0,
        .fontSize = 0.75f
    };

    auto currentWindow = LabelLayoutWindow::create(currentLayout, this);
    m_LayoutStuffCont->addChild(currentWindow);
    m_LayoutLines.push_back(currentWindow);

    currentWindow->setPositionBasedOnLayout(currentLayout);

    changeScrollSizeByBoxes();
}

void DTLayer::updateRunsAllowed(){
    if (m_MyLevelStats.currentBest != -1)
        StatsManager::saveData(m_MyLevelStats, m_Level);
    refreshStrings();
    RefreshText();
}

void DTLayer::FLAlert_Clicked(FLAlertLayer* layer, bool selected){
    if (ResetLayoutAlert == layer && selected){
        std::vector<LabelLayout> defaultLayout{
            {
                .labelName = "from 0",
                .text = "From 0:{nl}{f0}{nl} ",
                .line = 2,
                .position = 0,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Session",
                .text = "Session:{nl}{ssd}{nl}{s0}{nl} ",
                .line = 2,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Runs",
                .text = "Runs:{nl}{runs}{nl} ",
                .line = 3,
                .position = 0,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "SRuns",
                .text = "Session Runs:{nl}{sruns}{nl} ",
                .line = 3,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Title",
                .text = "{lvln}:",
                .line = 0,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 2,
                .fontSize = 1
            },
            {
                .labelName = "att",
                .text = "{att} attempts",
                .line = 1,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 1,
                .fontSize = 0.75f
            }
        };

        Save::setLayout(defaultLayout);

        Save::setNewBestColor({255, 255, 0});
        Save::setSessionBestColor({ 255, 136, 0 });

        createLayoutBlocks();
        onEditLayoutApply(nullptr);
    }
}

void DTLayer::onClose(cocos2d::CCObject*) {
    if (m_EditLayoutMenu->isVisible()){
        EditLayoutEnabled(false);
        if (m_LayoutStuffCont){
            m_LayoutStuffCont->removeMeAndCleanup();
            m_LayoutStuffCont = nullptr;
        }
        createLayoutBlocks();
        RefreshText();
    }
    else if (LevelSpecificSettingsLayer){
        //exit
        onSpecificSettings(nullptr);
    }
    else{
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        this->removeFromParentAndCleanup(true);
    }
}

void DTLayer::openGraphMenu(CCObject*){
    auto graph = DTGraphLayer::create(this);
    graph->setZOrder(100);
    this->addChild(graph);
}

void DTLayer::OnLinkButtonClicked(CCObject*){
    auto lLayer = DTLinkLayer::create(this);
    lLayer->setZOrder(100);
    this->addChild(lLayer);
}

void DTLayer::UpdateSharedStats(){
    m_SharedLevelStats = m_MyLevelStats;

    for (int i = 0; i < m_MyLevelStats.LinkedLevels.size(); i++)
    {
        auto currStats = StatsManager::getLevelStats(m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
        if (currStats.levelName == "Unknown name"){
            Notification::create("failed to get data for linked level - " + m_MyLevelStats.LinkedLevels[i])->show();
            continue;
        }

        m_SharedLevelStats.attempts += currStats.attempts;

        m_SharedLevelStats.sessions.reserve(m_SharedLevelStats.sessions.size() + std::distance(currStats.sessions.begin(),currStats.sessions.end()));
        m_SharedLevelStats.sessions.insert(m_SharedLevelStats.sessions.end(),currStats.sessions.begin(),currStats.sessions.end());

        for (int r = 0; r < m_MyLevelStats.RunsToSave.size(); r++)
        {
            bool addMeRun = true;
            for (int r2 = 0; r2 < currStats.RunsToSave.size(); r2++)
            {
                if (currStats.RunsToSave[r2] == m_MyLevelStats.RunsToSave[r])
                    addMeRun = false;
            }
            
            if (addMeRun)
                currStats.RunsToSave.push_back(m_MyLevelStats.RunsToSave[r]);
        }
        for (int r = 0; r < currStats.RunsToSave.size(); r++)
        {
            bool addMeRun = true;
            for (int r2 = 0; r2 < m_MyLevelStats.RunsToSave.size(); r2++)
            {
                if (m_MyLevelStats.RunsToSave[r2] == currStats.RunsToSave[r])
                    addMeRun = false;
            }
            
            if (addMeRun)
                m_MyLevelStats.RunsToSave.push_back(currStats.RunsToSave[r]);
        }

        std::ranges::sort(m_MyLevelStats.RunsToSave, [](const int a, const int b) {
            return a < b;
        });

        std::ranges::sort(currStats.RunsToSave, [](const int a, const int b) {
            return a < b;
        });

        if (m_MyLevelStats.currentBest != -1)
            StatsManager::saveData(m_MyLevelStats, m_Level);
        if (currStats.currentBest != -1)
            StatsManager::saveData(currStats, m_MyLevelStats.LinkedLevels[i]);
        
        for (const auto& [death, count] : currStats.deaths)
        {
            m_SharedLevelStats.deaths[death] += count;
        }
        
        for (const auto& [run, count] : currStats.runs)
        {
            m_SharedLevelStats.runs[run] += count;
        }

        if (m_SharedLevelStats.currentBest < currStats.currentBest)
            m_SharedLevelStats.currentBest = currStats.currentBest;

        m_SharedLevelStats.newBests.insert(currStats.newBests.begin(), currStats.newBests.end());
    }

    std::ranges::sort(m_SharedLevelStats.sessions, [](const Session a, const Session b) {
        return a.lastPlayed > b.lastPlayed;
    });

    m_SessionsAmount = m_SharedLevelStats.sessions.size();

    if (m_SessionSelectionInput)
        m_SessionSelectionInput->setString(fmt::format("{}/{}", m_SessionSelected, m_SessionsAmount));
}

void DTLayer::onSettings(CCObject*){
    geode::openSettingsPopup(Mod::get());
}

void DTLayer::onLayoutInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "The boxes (labels) here represent the text displayed.\n \nYou drag them around to change their order and <cy>double click</c> any of them for more options.\nClick the <cg>plus</c> button to add a new label.\nThe button on the bottom left will <cy>reset everything back to default.</c>", "Ok");
    alert->show();
}

void DTLayer::onCopyInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "Click a label to copy its text.", "Ok");
    alert->show();
}

void DTLayer::copyText(CCObject*)
{
    EditLayoutEnabled(true);

    isInCopyMenu = true;
    editLayoutApplyBtn->setEnabled(false);
    auto sprite = static_cast<CCSprite*>(editLayoutApplyBtn->getChildren()->objectAtIndex(0));
    sprite->setOpacity(100);
    static_cast<CCSprite*>(sprite->getChildren()->objectAtIndex(0))->setOpacity(100);
    resetLayoutButton->setVisible(false);
    //nbcColorPickerLabel->setVisible(false);
    //sbcColorPickerLabel->setVisible(false);

    //sbcColorPicker->setPosition({-500, -500});
    //nbcColorPicker->setPosition({-500, -500});

    for (int i = 0; i < m_LayoutLines.size(); i++)
    {
        auto IWindow = static_cast<LabelLayoutWindow*>(m_LayoutLines[i]);
        IWindow->setMoveEnabled(false);
    }

    layoutInfoButton->setVisible(false);
    addWindowButton->setVisible(false);
    copyInfoButton->setVisible(true);
}

void DTLayer::clickedWindow(CCNode* nwindow){
    auto window = static_cast<LabelLayoutWindow*>(nwindow);
    if (isInCopyMenu){
        EditLayoutEnabled(false);

        std::string toCopy = DTLayer::modifyString(window->m_MyLayout.text);

        for (int i = 0; i < toCopy.size(); i++)
        {
            if (toCopy[i] == '<' && toCopy.length() > i + 1){
                if (DTLayer::isKeyInIndex(toCopy, i + 1, "nbc>")){
                    toCopy.erase(i, 5);
                }
                if (DTLayer::isKeyInIndex(toCopy, i + 1, "sbc>")){
                    toCopy.erase(i, 5);
                }
            }
        }
        
        clipboard::write(toCopy);

        geode::Notification::create("Copied text from " + window->m_MyLayout.labelName, CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
    }   
}

void DTLayer::onResetLayout(CCObject*){
    ResetLayoutAlert = FLAlertLayer::create(this, "Warning!", "This will reset your layout back to the default layout.\n\nAre you sure you want to do this?", "Cancel", "Reset");
    ResetLayoutAlert->setZOrder(150);
    this->addChild(ResetLayoutAlert);
}

void DTLayer::editnbcColor(CCObject*){
    colorSelectnb = geode::ColorPickPopup::create(colorSpritenb->getColor());
    colorSelectnb->show();
    colorSelectsb->setColorTarget(colorSpritenb);
}

void DTLayer::editsbcColor(CCObject*){
    colorSelectsb = geode::ColorPickPopup::create(colorSpritesb->getColor());
    colorSelectsb->show();
    colorSelectsb->setColorTarget(colorSpritesb);
}

void DTLayer::onSpecificSettings(CCObject*){
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    if (runningMoveTransition) return;
    runningMoveTransition = true;

    if (LevelSpecificSettingsLayer){
        isExitingSSLayer = true;

        settingsButton->runAction(CCFadeIn::create(0.3f));
        LinkLevelsButton->runAction(CCFadeIn::create(0.3f));
        static_cast<ButtonSprite*>(copyTextButton->getChildren()->objectAtIndex(0))->m_label->runAction(CCFadeIn::create(0.3f));
        static_cast<ButtonSprite*>(copyTextButton->getChildren()->objectAtIndex(0))->m_BGSprite->runAction(CCFadeIn::create(0.3f));
        m_closeBtn->runAction(CCFadeIn::create(0.3f));
        m_EditLayoutBtn->runAction(CCFadeIn::create(0.3f));

        levelSettingsBSArrow->runAction(CCScaleTo::create(0.3f, 0.75f, 0.75f));

        LevelSpecificSettingsLayer->runAction(CCEaseInOut::create(CCScaleTo::create(0.3f, 0), 2));
        LevelSpecificSettingsLayer->runAction(CCSequence::create(CCEaseInOut::create(CCMoveTo::create(0.3f, ccp(310, winSize.height / 2)), 2), CCCallFunc::create(this, callfunc_selector(DTLayer::onMoveTransitionEnded)), nullptr));
        m_mainLayer->runAction(CCSequence::create(CCEaseInOut::create(CCMoveTo::create(0.3f, winSize / 2), 2), nullptr));
        LevelSpecificSettingsLayer->EnableTouch(false);

        return;
    }


    LevelSpecificSettingsLayer = DTLevelSpecificSettingsLayer::create({375, 280}, this);
    this->addChild(LevelSpecificSettingsLayer);
    LevelSpecificSettingsLayer->EnableTouch(false);

    settingsButton->setEnabled(false);
    settingsButton->runAction(CCFadeOut::create(0.3f));
    LinkLevelsButton->setEnabled(false);
    LinkLevelsButton->runAction(CCFadeOut::create(0.3f));
    copyTextButton->setEnabled(false);
    static_cast<ButtonSprite*>(copyTextButton->getChildren()->objectAtIndex(0))->m_label->runAction(CCFadeOut::create(0.3f));
    static_cast<ButtonSprite*>(copyTextButton->getChildren()->objectAtIndex(0))->m_BGSprite->runAction(CCFadeOut::create(0.3f));
    m_closeBtn->setEnabled(false);
    m_closeBtn->runAction(CCFadeOut::create(0.3f));
    m_EditLayoutBtn->setEnabled(false);
    m_EditLayoutBtn->runAction(CCFadeOut::create(0.3f));

    levelSettingsBSArrow->runAction(CCScaleTo::create(0.3f, -0.75f, 0.75f));

    LevelSpecificSettingsLayer->setPosition(104, 159);
    LevelSpecificSettingsLayer->setScale(0);
    LevelSpecificSettingsLayer->runAction(CCEaseInOut::create(CCScaleTo::create(0.3f, 1), 2));
    LevelSpecificSettingsLayer->runAction(CCSequence::create(CCEaseInOut::create(CCMoveTo::create(0.3f, ccp(205, winSize.height / 2)), 2), CCCallFunc::create(this, callfunc_selector(DTLayer::onMoveTransitionEnded)), nullptr));
    m_mainLayer->runAction(CCSequence::create(CCEaseInOut::create(CCMoveTo::create(0.3f, ccp((320 + m_size.width / 2.35f) / (winSize.width / (569 + m_size.width / 2.35f)), m_mainLayer->getPositionY())), 2), nullptr));
}

void DTLayer::onMoveTransitionEnded(){
    runningMoveTransition = false;
    if (isExitingSSLayer){
        isExitingSSLayer = false;
        LevelSpecificSettingsLayer->removeMeAndCleanup();
        LevelSpecificSettingsLayer = nullptr;
        settingsButton->setEnabled(true);
        LinkLevelsButton->setEnabled(true);
        copyTextButton->setEnabled(true);
        m_closeBtn->setEnabled(true);
        m_EditLayoutBtn->setEnabled(true);
    }
    else{
        LevelSpecificSettingsLayer->EnableTouch(true);
    }
}