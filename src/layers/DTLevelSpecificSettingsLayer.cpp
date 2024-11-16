#include "../layers/DTLevelSpecificSettingsLayer.hpp"
#include "../layers/DTExportImportLayer.hpp"
#include "../layers/DTLayer.hpp"
#include "../layers/RunAllowedCell.hpp"
#include "../hooks/DTCCTextInputNode.hpp"

DTLevelSpecificSettingsLayer* DTLevelSpecificSettingsLayer::create(const CCSize& size, DTLayer* const& _DTLayer) {
    auto ret = new DTLevelSpecificSettingsLayer();
    if (ret && ret->init(size, _DTLayer)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTLevelSpecificSettingsLayer::init(const CCSize& size, DTLayer* const& _DTLayer) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    myDTLayer = _DTLayer;
    m_Size = size;

    auto m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square01_001.png", {0.f, 0.f, 94.f, 94.f});
    m_bgSprite->setContentSize(size);
    this->addChild(m_bgSprite);

    auto title = CCLabelBMFont::create("Level Options", "goldFont.fnt");
    title->setPositionY(122);
    title->setScale(0.65f);
    this->addChild(title);

    //runs

    auto runsMenu = CCNode::create();
    runsMenu->setID("runs");
    runsMenu->setPosition({-size.width / 4 + 27, 35});
    this->addChild(runsMenu);

    auto runsMenuBG = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    runsMenuBG->setContentSize({210, 150});
    runsMenuBG->setOpacity(100);
    runsMenu->addChild(runsMenuBG);

    auto runsTitle = CCLabelBMFont::create("Run Management", "bigFont.fnt");
    runsTitle->setPositionY(65);
    runsTitle->setScale(0.4f);
    runsMenu->addChild(runsTitle);

    auto runsButtonsMenu = CCMenu::create();
    runsButtonsMenu->setPosition({0, 0});
    runsButtonsMenu->setZOrder(1);
    runsMenu->addChild(runsButtonsMenu);
    touchMenus.push_back(runsButtonsMenu);

    m_AddRunAllowedInput = TextInput::create(130, "Start %");
    m_AddRunAllowedInput->getInputNode()->setMaxLabelLength(3);
    m_AddRunAllowedInput->setCommonFilter(CommonFilter::Uint);
    m_AddRunAllowedInput->setDelegate(this);
    m_AddRunAllowedInput->setPosition({-62.5f, 40});
    m_AddRunAllowedInput->setScale(0.5f);
    runsMenu->addChild(m_AddRunAllowedInput);

    auto AddRunAllowedButtonS = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
    AddRunAllowedButtonS->setScale(0.75f);
    AddRunAllowedButton = CCMenuItemSpriteExtra::create(
        AddRunAllowedButtonS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::addRunAllowed)
    );
    AddRunAllowedButton->setPosition({-17.5f, 40});
    runsButtonsMenu->addChild(AddRunAllowedButton);

    auto runsAllowedBG = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", {0,0, 80, 80});
    runsAllowedBG->setContentSize({140, 125});
    runsAllowedBG->setPosition({-52.5f, -10});
    runsAllowedBG->setScale(0.65f);
    runsAllowedBG->setOpacity(100);
    runsMenu->addChild(runsAllowedBG);

    runsAllowedCellsCont = CCMenu::create();
    runsAllowedCellsCont->setContentSize(ccp(140, 125) * 0.65f - ccp(8, 8));
    runsAllowedCellsCont->setPosition(ccp(-52.5f, -10));
    runsAllowedCellsCont->setLayout(RowLayout::create()
        ->setCrossAxisOverflow(false)
        ->setGrowCrossAxis(true)
        ->setAxisAlignment(AxisAlignment::Even)
        ->setCrossAxisAlignment(AxisAlignment::End)
        ->setAutoScale(true)
    );
    runsMenu->addChild(runsAllowedCellsCont);
    touchMenus.push_back(runsAllowedCellsCont);

    for (int i = 0; i < myDTLayer->m_MyLevelStats.RunsToSave.size(); i++)
    {
        if (myDTLayer->m_MyLevelStats.RunsToSave[i] != -1){
            auto runACell = RunAllowedCell::create(myDTLayer->m_MyLevelStats.RunsToSave[i], 0.6f, std::bind(&DTLevelSpecificSettingsLayer::removeRunAllowed, this, std::placeholders::_1, std::placeholders::_2));
            runsAllowedCellsCont->addChild(runACell);
        }
    }

    runsAllowedCellsCont->updateLayout();

    auto RunAllowedInfoButtonS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    RunAllowedInfoButtonS->setScale(0.65f);
    auto RunAllowedInfoButton = CCMenuItemSpriteExtra::create(
        RunAllowedInfoButtonS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onRunsAInfo)
    );
    RunAllowedInfoButton->setPosition({100, 70});
    runsButtonsMenu->addChild(RunAllowedInfoButton);

    auto checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    allRunsToggle = CCMenuItemToggler::create(
        checkOff,
        checkOn,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::OnToggleAllRuns)
    );

    if (myDTLayer->m_MyLevelStats.RunsToSave.size())
        if (myDTLayer->m_MyLevelStats.RunsToSave[0] == -1){
            allRunsToggle->toggle(true);
        }

    allRunsToggle->setPosition({52.5f, 32});
    allRunsToggle->setScale(0.7f);
    runsButtonsMenu->addChild(allRunsToggle);

    auto allRunsToggleLabel = CCLabelBMFont::create("Track any run", "bigFont.fnt");
    allRunsToggleLabel->setPosition({52.5f, 14});
    allRunsToggleLabel->setScale(0.325f);
    runsButtonsMenu->addChild(allRunsToggleLabel);

    auto DeleteUnusedButtonS = ButtonSprite::create("Delete Unused", "bigFont.fnt", "GJ_button_06.png");
    DeleteUnusedButtonS->setScale(0.3f);
    DeleteUnusedButton = CCMenuItemSpriteExtra::create(
        DeleteUnusedButtonS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::deleteUnused)
    );
    DeleteUnusedButton->setPosition({-52.5f, -60});
    runsButtonsMenu->addChild(DeleteUnusedButton);

    if (allRunsToggle->isOn()){
        runsAllowedCellsCont->setEnabled(false);
        DeleteUnusedButton->setEnabled(false);
        auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
        auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
        DeleteUnusedButtonBG->setOpacity(100);
        DeleteUnusedButtonLabel->setOpacity(100);

        CCObject* child;
        CCARRAY_FOREACH(runsAllowedCellsCont->getChildren(), child){
            if (auto b = typeinfo_cast<RunAllowedCell*>(child)){
                b->buttonSprite->m_BGSprite->setOpacity(100);
                b->buttonSprite->m_label->setOpacity(100);
            }
        }

        m_AddRunAllowedInput->setEnabled(false);

        AddRunAllowedButton->setEnabled(false);
        AddRunAllowedButton->setOpacity(100);
    }
    if (runsAllowedCellsCont->getChildrenCount() == 0){
        DeleteUnusedButton->setEnabled(false);
        auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
        auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
        DeleteUnusedButtonBG->setOpacity(100);
        DeleteUnusedButtonLabel->setOpacity(100);
    }

    auto HideUptoInputLabel = CCLabelBMFont::create("Hide Up To", "bigFont.fnt");
    HideUptoInputLabel->setPosition({52.5f, -30});
    HideUptoInputLabel->setScale(0.45f);
    runsMenu->addChild(HideUptoInputLabel);

    HideUptoInput = TextInput::create(165, "hideto %");
    HideUptoInput->getInputNode()->setMaxLabelLength(3);
    HideUptoInput->setCommonFilter(CommonFilter::Uint);
    HideUptoInput->setDelegate(this);
    HideUptoInput->setPosition({52.5f, -47});
    HideUptoInput->setString(std::to_string(myDTLayer->m_MyLevelStats.hideUpto));
    HideUptoInput->setScale(0.5f);
    runsMenu->addChild(HideUptoInput);

    //saveManagement

    auto saveManagmentMenu = CCNode::create();
    saveManagmentMenu->setID("save-management");
    saveManagmentMenu->setPosition({-size.width / 4 + 27, -85});
    this->addChild(saveManagmentMenu);

    auto saveManagmentMenuBG = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    saveManagmentMenuBG->setContentSize({210, 80});
    saveManagmentMenuBG->setOpacity(100);
    saveManagmentMenu->addChild(saveManagmentMenuBG);

    auto saveManagmentTitle = CCLabelBMFont::create("Save Management", "bigFont.fnt");
    saveManagmentTitle->setPositionY(30);
    saveManagmentTitle->setScale(0.4f);
    saveManagmentMenu->addChild(saveManagmentTitle);

    auto saveManagmentButtonsMenu = CCMenu::create();
    saveManagmentButtonsMenu->setPosition({0, 0});
    saveManagmentButtonsMenu->setZOrder(1);
    saveManagmentMenu->addChild(saveManagmentButtonsMenu);
    touchMenus.push_back(saveManagmentButtonsMenu);

    auto saveManagmentInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    saveManagmentInfoBS->setScale(0.65f);
    auto saveManagmentInfoButton = CCMenuItemSpriteExtra::create(
        saveManagmentInfoBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onSaveManagementInfo)
    );
    saveManagmentInfoButton->setPosition({100, 35});
    saveManagmentButtonsMenu->addChild(saveManagmentInfoButton);

    //export/import
    auto exportBS = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
    auto exportBS1 = CCSprite::createWithSpriteFrameName("geode.loader/install.png");
    exportBS1->setPosition(exportBS->getContentSize() / 2);
    exportBS1->setScale(1.2f);
    exportBS->addChild(exportBS1);
    exportBS->setScale(0.75f);
    auto exportButton = CCMenuItemSpriteExtra::create(
        exportBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onExportClicked)
    );
    exportButton->setAnchorPoint({0.5f, 0.5f});
    exportButton->setPosition({0, 0});
    saveManagmentButtonsMenu->addChild(exportButton);

    auto exportButtonLabel = CCLabelBMFont::create("Import\nExport", "bigFont.fnt");
    exportButtonLabel->setPosition({0, -25});
    exportButtonLabel->setScale(0.275f);
    exportButtonLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    saveManagmentMenu->addChild(exportButtonLabel);
    
    //delete
    auto deleteCurrentSaveBS = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    deleteCurrentSaveBS->setScale(0.825f);
    auto deleteCurrentSaveButton = CCMenuItemSpriteExtra::create(
        deleteCurrentSaveBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onCurrentDeleteClicked)
    );
    deleteCurrentSaveButton->setPosition({-60, 0});
    saveManagmentButtonsMenu->addChild(deleteCurrentSaveButton);

    auto deleteCurrentSaveButtonLabel = CCLabelBMFont::create("Delete\nSave", "bigFont.fnt");
    deleteCurrentSaveButtonLabel->setPosition({-60, -25});
    deleteCurrentSaveButtonLabel->setScale(0.275f);
    deleteCurrentSaveButtonLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    saveManagmentMenu->addChild(deleteCurrentSaveButtonLabel);

    //revert
    auto revertBackupBS = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    revertBackupBS->setScale(0.575f);
    auto revertBackupButton = CCMenuItemSpriteExtra::create(
        revertBackupBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onRevertClicked)
    );
    revertBackupButton->setPosition({60, 0});
    saveManagmentButtonsMenu->addChild(revertBackupButton);

    auto revertBackupButtonLabel = CCLabelBMFont::create("Revert\nBackup", "bigFont.fnt");
    revertBackupButtonLabel->setPosition({60, -25});
    revertBackupButtonLabel->setScale(0.275f);
    revertBackupButtonLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    saveManagmentMenu->addChild(revertBackupButtonLabel);

    //runEditing

    auto runEditingMenu = CCNode::create();
    runEditingMenu->setID("run-editing");
    runEditingMenu->setPosition({size.width / 4 + 14, -7.5f});
    this->addChild(runEditingMenu);

    auto runEditingMenuBG = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    runEditingMenuBG->setContentSize({120, 235});
    runEditingMenuBG->setOpacity(100);
    runEditingMenu->addChild(runEditingMenuBG);

    auto runEditingTitle = CCLabelBMFont::create("Run Editing", "bigFont.fnt");
    runEditingTitle->setPositionY(107);
    runEditingTitle->setScale(0.4f);
    runEditingMenu->addChild(runEditingTitle);

    auto modifyRunsButtonsMenu = CCMenu::create();
    modifyRunsButtonsMenu->setPosition({0, 0});
    runEditingMenu->addChild(modifyRunsButtonsMenu);

    auto addRunsInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    addRunsInfoBS->setScale(0.65f);
    auto addRunsInfoButton = CCMenuItemSpriteExtra::create(
        addRunsInfoBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onModRunsInfo)
    );
    addRunsInfoButton->setPosition({56, 112.5f});
    modifyRunsButtonsMenu->addChild(addRunsInfoButton);

    auto addFZRunInputTitle = CCLabelBMFont::create("From 0:", "bigFont.fnt");
    addFZRunInputTitle->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    addFZRunInputTitle->setPosition({0, 77});
    addFZRunInputTitle->setScale(0.55f);
    runEditingMenu->addChild(addFZRunInputTitle);

    addFZRunInput = TextInput::create(175, "Run %");
    addFZRunInput->setPosition({0, 58});
    addFZRunInput->setScale(0.55f);
    addFZRunInput->setDelegate(this);
    addFZRunInput->setCommonFilter(CommonFilter::Uint);
    runEditingMenu->addChild(addFZRunInput);

    auto addFZRunsBS = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
    auto addFZRunsButton = CCMenuItemSpriteExtra::create(
        addFZRunsBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onAddedFZRun)
    );
    addFZRunsButton->setPosition({-18, 37});
    modifyRunsButtonsMenu->addChild(addFZRunsButton);

    auto removeFZRunsBS = CCSprite::create("minus_button.png"_spr);
    auto removeFZRunsButton = CCMenuItemSpriteExtra::create(
        removeFZRunsBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onRemovedFZRun)
    );
    removeFZRunsButton->setPosition({17.5f, 36.5f});
    modifyRunsButtonsMenu->addChild(removeFZRunsButton);

    auto addRunInputTitle = CCLabelBMFont::create("Run:", "bigFont.fnt");
    addRunInputTitle->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    addRunInputTitle->setPosition({0, 8});
    addRunInputTitle->setScale(0.55f);
    runEditingMenu->addChild(addRunInputTitle);
    

    addRunStartInput = TextInput::create(175, "Run Start %");
    addRunStartInput->setPosition({0, -11});
    addRunStartInput->setScale(0.5f);
    addRunStartInput->setDelegate(this);
    addRunStartInput->setCommonFilter(CommonFilter::Uint);
    static_cast<DTCCTextInputNode*>(addRunStartInput->getInputNode())->setCallback(callfunc_selector(DTLevelSpecificSettingsLayer::addRunStartInputKeyDown), this);
    runEditingMenu->addChild(addRunStartInput);

    

    runSeparator = CCLabelBMFont::create("-", "bigFont.fnt");
    runSeparator->setPosition({0, -23});
    runSeparator->setScale(0.8f);
    runSeparator->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    runEditingMenu->addChild(runSeparator);

    addRunEndInput = TextInput::create(175, "Run End %");
    addRunEndInput->setPosition({0, -40});
    addRunEndInput->setScale(0.5f);
    addRunEndInput->setDelegate(this);
    addRunEndInput->setCommonFilter(CommonFilter::Uint);
    runEditingMenu->addChild(addRunEndInput);

    auto addRunsBS = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
    auto addRunsButton = CCMenuItemSpriteExtra::create(
        addRunsBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onAddedRun)
    );
    addRunsButton->setPosition({-18, -60});
    modifyRunsButtonsMenu->addChild(addRunsButton);

    auto removeRunsBS = CCSprite::create("minus_button.png"_spr);
    auto removeRunsButton = CCMenuItemSpriteExtra::create(
        removeRunsBS,
        nullptr,
        this,
        menu_selector(DTLevelSpecificSettingsLayer::onRemovedRun)
    );
    removeRunsButton->setPosition({17.5f, -60.5f});
    modifyRunsButtonsMenu->addChild(removeRunsButton);

    auto runsAmountlabel = CCLabelBMFont::create("Amount", "bigFont.fnt");
    runsAmountlabel->setPosition({0, -84});
    runsAmountlabel->setScale(0.5f);
    runsAmountlabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    runEditingMenu->addChild(runsAmountlabel);

    runsAmountInput = TextInput::create(130, "Amount");
    runsAmountInput->setPosition({0, -102});
    runsAmountInput->setScale(0.5f);
    runsAmountInput->setDelegate(this);
    runsAmountInput->setCommonFilter(CommonFilter::Uint);
    runsAmountInput->setString("1");
    runEditingMenu->addChild(runsAmountInput);

    return true;
}

//-- runs functions --

void DTLevelSpecificSettingsLayer::addRunAllowed(CCObject*){
    int startPrecent = -1;
    if (m_AddRunAllowedInput->getString() != ""){
        auto res = utils::numFromString<int>(m_AddRunAllowedInput->getString());
        startPrecent = res.unwrapOr(-1);
    }

    if (startPrecent == -1) return;

    bool doesExist = false;
    for (int i = 0; i < myDTLayer->m_MyLevelStats.RunsToSave.size(); i++)
    {
        if (myDTLayer->m_MyLevelStats.RunsToSave[i] == startPrecent)
            doesExist = true;
    }
    
    if (!doesExist){
        myDTLayer->m_MyLevelStats.RunsToSave.push_back(startPrecent);

        std::ranges::sort(myDTLayer->m_MyLevelStats.RunsToSave, [](const int a, const int b) {
            return a < b; // true --> A before B
        });

        for (int i = 0; i < myDTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
        {
            auto currStats = StatsManager::getLevelStats(myDTLayer->m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
            if (currStats.levelName == "Unknown name"){
                Notification::create("Failed syncing data with linked level - " + myDTLayer->m_MyLevelStats.LinkedLevels[i])->show();
                continue;
            }

            bool doesExist2 = false;
            for (int r = 0; r < currStats.RunsToSave.size(); r++)
            {
                if (currStats.RunsToSave[r] == startPrecent)
                    doesExist2 = true;
            }

            if (!doesExist2 && currStats.currentBest != -1){
                currStats.RunsToSave.push_back(startPrecent);

                std::ranges::sort(currStats.RunsToSave, [](const int a, const int b) {
                    return a < b; // true --> A before B
                });

                StatsManager::saveData(currStats, myDTLayer->m_MyLevelStats.LinkedLevels[i]);
            }
        }

        auto runACell = RunAllowedCell::create(startPrecent, 0.6f, std::bind(&DTLevelSpecificSettingsLayer::removeRunAllowed, this, std::placeholders::_1, std::placeholders::_2));
        runsAllowedCellsCont->addChild(runACell);
        runsAllowedCellsCont->updateLayout();
        
        myDTLayer->updateRunsAllowed();

        DeleteUnusedButton->setEnabled(true);
        auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
        auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
        DeleteUnusedButtonBG->setOpacity(255);
        DeleteUnusedButtonLabel->setOpacity(255);
    }
}

void DTLevelSpecificSettingsLayer::removeRunAllowed(const int& precent, CCNode* cell){
    LevelStats myStats = myDTLayer->m_MyLevelStats;

    for (int i = 0; i < myStats.RunsToSave.size(); i++)
    {
        if (myStats.RunsToSave[i] == precent){
            myStats.RunsToSave.erase(std::next(myStats.RunsToSave.begin(), i));
            break;
        }
    }

    std::ranges::sort(myStats.RunsToSave, [](const int a, const int b) {
        return a < b; // true --> A before B
    });

    for (int i = 0; i < myStats.LinkedLevels.size(); i++){
        auto currStats = StatsManager::getLevelStats(myStats.LinkedLevels[i]).unwrapOrDefault();
        if (currStats.levelName == "Unknown name"){
            Notification::create("Failed syncing data with linked level - " + myStats.LinkedLevels[i])->show();
            continue;
        }

        bool doesExist2 = false;
        for (int r = 0; r < currStats.RunsToSave.size(); r++)
        {
            if (currStats.RunsToSave[r] == precent){
                currStats.RunsToSave.erase(std::next(currStats.RunsToSave.begin(), r));
                break;
            }
        }
        if (currStats.currentBest != -1)
            StatsManager::saveData(currStats, myStats.LinkedLevels[i]);
    }

    myDTLayer->m_MyLevelStats = myStats;
    
    myDTLayer->updateRunsAllowed();

    cell->removeMeAndCleanup();
    runsAllowedCellsCont->updateLayout();

    DeleteUnusedButton->setEnabled(runsAllowedCellsCont->getChildrenCount() != 0);
    auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
    auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
    if (DeleteUnusedButton->isEnabled()){
        DeleteUnusedButtonBG->setOpacity(255);
        DeleteUnusedButtonLabel->setOpacity(255);
    }
    else{
        DeleteUnusedButtonBG->setOpacity(100);
        DeleteUnusedButtonLabel->setOpacity(100);
    }
}

void DTLevelSpecificSettingsLayer::OnToggleAllRuns(CCObject*){
    if (allRunsToggle->isOn()){
        
        if (myDTLayer->m_MyLevelStats.RunsToSave.size()){
            if (myDTLayer->m_MyLevelStats.RunsToSave[0] == -1){
                myDTLayer->m_MyLevelStats.RunsToSave.erase(myDTLayer->m_MyLevelStats.RunsToSave.begin());

                for (int i = 0; i < myDTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
                {
                    auto currStats = StatsManager::getLevelStats(myDTLayer->m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
                    if (currStats.levelName == "Unknown name"){
                        Notification::create("Failed syncing data with linked level - " + myDTLayer->m_MyLevelStats.LinkedLevels[i])->show();
                        continue;
                    }

                    if (currStats.RunsToSave.size() != 0 && currStats.currentBest != -1){
                        if (currStats.RunsToSave[0] == -1){
                            currStats.RunsToSave.erase(std::next(currStats.RunsToSave.begin(), 0));
                        }
                        StatsManager::saveData(currStats, myDTLayer->m_MyLevelStats.LinkedLevels[i]);
                    }
                }

                myDTLayer->updateRunsAllowed();
            }
        }
    }
    else{
        myDTLayer->m_MyLevelStats.RunsToSave.insert(myDTLayer->m_MyLevelStats.RunsToSave.begin(), -1);
        myDTLayer->updateRunsAllowed();
    }

    runsAllowedCellsCont->setEnabled(allRunsToggle->isOn());
    
    CCObject* child;
    CCARRAY_FOREACH(runsAllowedCellsCont->getChildren(), child){
        if (auto b = typeinfo_cast<RunAllowedCell*>(child)){
            if (runsAllowedCellsCont->isEnabled()){
                b->buttonSprite->m_BGSprite->setOpacity(255);
                b->buttonSprite->m_label->setOpacity(255);
            }
            else{
                b->buttonSprite->m_BGSprite->setOpacity(100);
                b->buttonSprite->m_label->setOpacity(100);
            }
        }
    }
    
    m_AddRunAllowedInput->setEnabled(allRunsToggle->isOn());

    AddRunAllowedButton->setEnabled(allRunsToggle->isOn());
    if (AddRunAllowedButton->isEnabled())
        AddRunAllowedButton->setOpacity(255);
    else
        AddRunAllowedButton->setOpacity(100);

    DeleteUnusedButton->setEnabled(allRunsToggle->isOn() && runsAllowedCellsCont->getChildrenCount() != 0);
    auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
    auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
    if (DeleteUnusedButton->isEnabled()){
        DeleteUnusedButtonBG->setOpacity(255);
        DeleteUnusedButtonLabel->setOpacity(255);
    }
    else{
        DeleteUnusedButtonBG->setOpacity(100);
        DeleteUnusedButtonLabel->setOpacity(100);
    }
}

void DTLevelSpecificSettingsLayer::deleteUnused(CCObject*){
    m_RunDeleteAlert = FLAlertLayer::create(this, "Warning!", "This will delete all saved runs that were not added to the list of runs to track, please make sure you have all of the percents you want on the tracked runs list before doing this.", "Cancel", "Delete");
    m_RunDeleteAlert->show();
}

void DTLevelSpecificSettingsLayer::onRunsAInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", fmt::format("{}\n{}\n{}\n{}",
    "- <cg>Runs list</c>, any run starting from a percentage within this list will be tracked.",
    "- <cr>Delete unused</c> will delete all runs that don't start from a percentage that's on the <cg>runs list</c>.",
    "- <cy>Track any run</c> will let the mod track any run starting at any percentage. This will disable the <cg>runs list</c>.",
    "- <cl>Hide up to</c> will hide any run from 0% that's below the inputted percentage."
    ), "Ok");
    alert->show();
}



//-- Save Manegment Functions --
void DTLevelSpecificSettingsLayer::onCurrentDeleteClicked(CCObject*){
    currDeleteAlert = FLAlertLayer::create(this, "WARNING!", "Doing this will delete <cr>ALL</c> of your saved progress on this level.", "Cancel", "Delete");
    currDeleteAlert->show();
}

void DTLevelSpecificSettingsLayer::onRevertClicked(CCObject*){
    revertAlert = FLAlertLayer::create(this, "WARNING!", "Doing this will <cy>revert your progress to the last created backup</c>, It's recommended to do this if your json doesn't load or is corrupted.", "Cancel", "Revert");
    revertAlert->show();
}

void DTLevelSpecificSettingsLayer::onExportClicked(CCObject*){
    auto EILayer = DTExportImportLayer::create(myDTLayer);
    EILayer->setZOrder(100);
    this->getParent()->addChild(EILayer);
}

void DTLevelSpecificSettingsLayer::onSaveManagementInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", fmt::format("{}\n{}\n{}",
    "- <cr>Delete save</c> deletes <cr>ALL</c> progress from this level.",
    "- <cg>Import/Export</c> will let you <cl>export</c> your runs into a .txt file, or <cp>import</c> all your runs into a new text file.",
    "- <cy>Revert backup</c> will revert the save to the latest backup, good in the case of your save file being corrupt."
    ), "Ok");
    alert->show();
}

//-- Runs Management Functions--

void DTLevelSpecificSettingsLayer::onAddedFZRun(CCObject*){
    if (myDTLayer->m_MyLevelStats.currentBest == -1) return;

    int amount = 1;
    if (runsAmountInput->getString() != ""){
        auto res = utils::numFromString<int>(runsAmountInput->getString());
        amount = res.unwrapOr(1);
    }

    int precent = 0;
    if (addFZRunInput->getString() != ""){
        auto res = utils::numFromString<int>(addFZRunInput->getString());
        precent = res.unwrapOr(0);
    }
    
    myDTLayer->m_MyLevelStats.deaths[std::to_string(precent)] += amount;

    StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
    myDTLayer->UpdateSharedStats();
    myDTLayer->refreshStrings();
    myDTLayer->RefreshText();
}

void DTLevelSpecificSettingsLayer::onRemovedFZRun(CCObject*){

    if (myDTLayer->m_MyLevelStats.currentBest == -1) return;

    int amount = 1;
    if (runsAmountInput->getString() != ""){
        auto res = utils::numFromString<int>(runsAmountInput->getString());
        amount = res.unwrapOr(1);
    }
        
    int precent = 0;
    if (addFZRunInput->getString() != ""){
        auto res = utils::numFromString<int>(addFZRunInput->getString());
        precent = res.unwrapOr(0);
    }
        
    if (myDTLayer->m_MyLevelStats.deaths.contains(std::to_string(precent))){
        myDTLayer->m_MyLevelStats.deaths[std::to_string(precent)] -= amount;
        if (myDTLayer->m_MyLevelStats.deaths[std::to_string(precent)] <= 0){
            myDTLayer->m_MyLevelStats.deaths.erase(std::to_string(precent));
        }

        StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
    }
    else{
        for (int i = 0; i < myDTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
        {
            auto lStats = StatsManager::getLevelStats(myDTLayer->m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
            if (lStats.levelName == "Unknown name"){
                Notification::create("Failed syncing data with linked level - " + myDTLayer->m_MyLevelStats.LinkedLevels[i])->show();
                continue;
            }

            if (lStats.deaths.contains(std::to_string(precent))){
                lStats.deaths[std::to_string(precent)] -= amount;
                if (lStats.deaths[std::to_string(precent)] <= 0){
                    lStats.deaths.erase(std::to_string(precent));
                }

                StatsManager::saveData(lStats, myDTLayer->m_MyLevelStats.LinkedLevels[i]);
                break;
            }
        }
        
    }
    
    myDTLayer->UpdateSharedStats();
    myDTLayer->refreshStrings();
    myDTLayer->RefreshText();
}

void DTLevelSpecificSettingsLayer::onAddedRun(CCObject*){
    if (myDTLayer->m_MyLevelStats.currentBest == -1) return;

    int amount = 1;
    if (runsAmountInput->getString() != ""){
        auto res = utils::numFromString<int>(runsAmountInput->getString());
        amount = res.unwrapOr(1);
    }

    Run r{
        0,
        0
    };

    if (addRunStartInput->getString() != ""){
        auto res = utils::numFromString<int>(addRunStartInput->getString());
        r.start = res.unwrapOr(0);
    }
        

    if (addRunEndInput->getString() != ""){
        auto res = utils::numFromString<int>(addRunEndInput->getString());
        r.end = res.unwrapOr(0);
    }
    
    if (r.start > r.end) return;

    std::string runKey = fmt::format("{}-{}", r.start, r.end);
    
    myDTLayer->m_MyLevelStats.runs[runKey] += amount;

    StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
    myDTLayer->UpdateSharedStats();
    myDTLayer->refreshStrings();
    myDTLayer->RefreshText();
}

void DTLevelSpecificSettingsLayer::onRemovedRun(CCObject*){
    if (myDTLayer->m_MyLevelStats.currentBest == -1) return;

    int amount = 1;
    if (runsAmountInput->getString() != ""){
            auto res = utils::numFromString<int>(runsAmountInput->getString());
            amount = res.unwrapOr(1);
        }
    
    Run r{
        0,
        0
    };

    if (addRunStartInput->getString() != ""){
            auto res = utils::numFromString<int>(addRunStartInput->getString());
            r.start = res.unwrapOr(0);
        }

    if (addRunEndInput->getString() != ""){
            auto res = utils::numFromString<int>(addRunEndInput->getString());
            r.end = res.unwrapOr(0);
        }
        

    if (r.start > r.end) return;

    std::string runKey = fmt::format("{}-{}", r.start, r.end);
    
    if (myDTLayer->m_MyLevelStats.runs.contains(runKey)){
        myDTLayer->m_MyLevelStats.runs[runKey] -= amount;
        if (myDTLayer->m_MyLevelStats.runs[runKey] <= 0){
            myDTLayer->m_MyLevelStats.runs.erase(runKey);
        }

        StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
    }
    else{
        for (int i = 0; i < myDTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
        {
            auto lStats = StatsManager::getLevelStats(myDTLayer->m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
            if (lStats.levelName == "Unknown name"){
                Notification::create("Failed syncing data with linked level - " + myDTLayer->m_MyLevelStats.LinkedLevels[i])->show();
                continue;
            }

            if (lStats.runs.contains(runKey)){
                lStats.runs[runKey] -= amount;
                if (lStats.runs[runKey] <= 0){
                    lStats.runs.erase(runKey);
                }

                StatsManager::saveData(lStats, myDTLayer->m_MyLevelStats.LinkedLevels[i]);
                break;
            }
        }
        
    }

    
    myDTLayer->UpdateSharedStats();
    myDTLayer->refreshStrings();
    myDTLayer->RefreshText();
}

void DTLevelSpecificSettingsLayer::onModRunsInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", fmt::format("{}\n{}\n{}",
    "Here you can manually add/remove runs from the save file!",
    "You can add both runs and from 0 attempts, click the <cg>(+)</c> or <cr>(-)</c> buttons to add or remove runs.",
    "The <cy>amount</c> added/removed at a time is determined by the <cy>amount</c> text field below."
    ), "Ok");
    alert->show();
}


//-- overall functions --
void DTLevelSpecificSettingsLayer::FLAlert_Clicked(FLAlertLayer* layer, bool selected){
    if (m_RunDeleteAlert == layer && selected){
        if (myDTLayer->m_MyLevelStats.currentBest == -1) return;

        for (auto it = myDTLayer->m_MyLevelStats.runs.cbegin(); it != myDTLayer->m_MyLevelStats.runs.cend();)
        {
            bool erase = true;
            for (int i = 0; i < myDTLayer->m_MyLevelStats.RunsToSave.size(); i++)
            {
                
                if (StatsManager::splitRunKey(it->first).start == myDTLayer->m_MyLevelStats.RunsToSave[i]){
                    erase = false;
                }
            }
            if (erase){
                myDTLayer->m_MyLevelStats.runs.erase(it++);
            }
            else{
                ++it;
            }
        }

        for (int i = 0; i < myDTLayer->m_MyLevelStats.sessions.size(); i++)
        {
            for (auto it = myDTLayer->m_MyLevelStats.sessions[i].runs.cbegin(); it != myDTLayer->m_MyLevelStats.sessions[i].runs.cend();)
            {
                bool erase = true;
                for (int i = 0; i < myDTLayer->m_MyLevelStats.RunsToSave.size(); i++)
                {
                    
                    if (StatsManager::splitRunKey(it->first).start == myDTLayer->m_MyLevelStats.RunsToSave[i]){
                        erase = false;
                    }
                }
                if (erase){
                    myDTLayer->m_MyLevelStats.sessions[i].runs.erase(it++);
                }
                else{
                    ++it;
                }
            }
        }
        

        StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
        myDTLayer->UpdateSharedStats();
        myDTLayer->refreshStrings();
        myDTLayer->RefreshText();
    }

    if (currDeleteAlert == layer && selected){
        auto path = StatsManager::getLevelSaveFilePath(myDTLayer->m_Level).unwrapOrDefault();
        if (path.empty()){
            Notification::create("Failed to delete level data! (invalid level path!)")->show();
            return;
        }
        std::filesystem::remove(path);

        LevelStats stats;

        stats.attempts = myDTLayer->m_Level->m_attempts;
        stats.levelName = myDTLayer->m_Level->m_levelName;
        stats.currentBest = 0;
        stats.difficulty = StatsManager::getDifficulty(myDTLayer->m_Level);

        Session startingSession;
        startingSession.lastPlayed = -2;
        startingSession.currentBest = -1;

        stats.sessions.push_back(startingSession);

        StatsManager::saveData(stats, myDTLayer->m_Level);

        auto alert = FLAlertLayer::create("Success!", "All progress has been deleted.", "Ok");
        alert->setZOrder(150);
        this->getParent()->addChild(alert);

        myDTLayer->LevelSpecificSettingsLayer = nullptr;
        myDTLayer->onClose(nullptr);
    }

    if (revertAlert == layer && selected){
        auto stats = StatsManager::getBackupStats(myDTLayer->m_Level).unwrapOrDefault();
        if (stats.levelName == "Unknown name"){
            Notification::create("Failed to get backup stats! (" + std::to_string(myDTLayer->m_Level->m_levelID.value()) + ")")->show();
            return;
        }

        if (stats.currentBest == -1){
            auto alert = FLAlertLayer::create("Failed", "No valid backup found :(\n\nIf your data doesn't load then it is probably corrupt, and in that case it is best to <cr>delete the current level using the delete button.</c>", "Ok");
            alert->setZOrder(150);
            this->addChild(alert);
            return;
        }
        
        StatsManager::saveData(stats, myDTLayer->m_Level);

        auto alert = FLAlertLayer::create("Success!", "Progress reverted to the latest backup.", "Ok");
        alert->setZOrder(150);
        this->getParent()->addChild(alert);

        myDTLayer->LevelSpecificSettingsLayer = nullptr;
        myDTLayer->onClose(nullptr);
    }
}

void DTLevelSpecificSettingsLayer::EnableTouch(bool b){
    for (int i = 0; i < touchMenus.size(); i++)
    {
        if (b && allRunsToggle->isOn() && touchMenus[i] == runsAllowedCellsCont){
            touchMenus[i]->setEnabled(false);
            DeleteUnusedButton->setEnabled(false);
            auto DeleteUnusedButtonBG = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_BGSprite;
            auto DeleteUnusedButtonLabel = static_cast<ButtonSprite*>(DeleteUnusedButton->getChildren()->objectAtIndex(0))->m_label;
            DeleteUnusedButtonBG->setOpacity(100);
            DeleteUnusedButtonLabel->setOpacity(100);

            CCObject* child;
            CCARRAY_FOREACH(runsAllowedCellsCont->getChildren(), child){
                if (auto b = typeinfo_cast<RunAllowedCell*>(child)){
                    b->buttonSprite->m_BGSprite->setOpacity(100);
                    b->buttonSprite->m_label->setOpacity(100);
                }
            }
        }
        else
            touchMenus[i]->setEnabled(b);
    }
    
}

void DTLevelSpecificSettingsLayer::textChanged(CCTextInputNode* input){
    if (input == m_AddRunAllowedInput->getInputNode()){

        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }

        if (res > 100){
            res = 100;
            input->setString("100");
        }

    }

    if (input == HideUptoInput->getInputNode()){

        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }

        if (res > 100){
            res = 100;
            input->setString("100");
        }

        for (int i = 0; i < myDTLayer->m_MyLevelStats.LinkedLevels.size(); i++)
        {
            auto currStats = StatsManager::getLevelStats(myDTLayer->m_MyLevelStats.LinkedLevels[i]).unwrapOrDefault();
            if (currStats.levelName == "Unknown name"){
                Notification::create("Failed syncing data with linked level - " + myDTLayer->m_MyLevelStats.LinkedLevels[i])->show();
                return;
            }

            currStats.hideUpto = res;
            StatsManager::saveData(currStats, myDTLayer->m_MyLevelStats.LinkedLevels[i]);
        }

        myDTLayer->m_MyLevelStats.hideUpto = res;

        StatsManager::saveData(myDTLayer->m_MyLevelStats, myDTLayer->m_Level);
        myDTLayer->refreshStrings();
        myDTLayer->RefreshText();
    }

    if (input == addFZRunInput->getInputNode()){
        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }

        if (res > 100){
            res = 100;
            input->setString("100");
        }

    }

    if (input == addRunStartInput->getInputNode()){

        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }
            
        
        if (res > 100){
            res = 100;
            input->setString("100");
        }
    }

    if (input == addRunEndInput->getInputNode()){

        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }
        
        if (res > 100){
            res = 100;
            input->setString("100");
        }
    }

    if (input == runsAmountInput->getInputNode()){

        int res = 0;
        if (input->getString() != ""){
            auto resNum = utils::numFromString<int>(input->getString());
            res = resNum.unwrapOr(0);
        }

        if (res == 0){
            res = 1;
            input->setString("1");
        }

        if (res > 100){
            res = 100;
            input->setString("100");
        }
    }
}

void DTLevelSpecificSettingsLayer::addRunStartInputKeyDown(){
    auto field = static_cast<DTCCTextInputNode*>(addRunStartInput->getInputNode());

    std::string keyEntered = field->m_fields->lastText;

    if (addRunStartInput->getInputNode()->m_selected && keyEntered == "-"){
        addRunStartInput->defocus();
        addRunEndInput->focus();
        runSeparator->setColor({ 87, 255, 87 });
        runSeparator->runAction(CCTintTo::create(0.4f, 255, 255, 255));
    }
}
