#include "../layers/DTExportImportLayer.hpp"
#include "../utils/Save.hpp"
#include "../layers/confirmImportLayer.hpp"

DTExportImportLayer* DTExportImportLayer::create(DTLayer* layer) {
    auto ret = new DTExportImportLayer();
    if (ret && ret->init(226, 137, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTExportImportLayer::setup(DTLayer* layer) {

    m_DTLayer = layer;

    auto AlighmentNode = CCNode::create();
    AlighmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(AlighmentNode);

    //export
    auto exportAlighmentNode = CCNode::create();
    exportAlighmentNode->setPosition({132, -54});
    AlighmentNode->addChild(exportAlighmentNode);

    auto exportMenu = CCMenu::create();
    exportMenu->setPosition({-180, 47});
    exportMenu->setZOrder(1);
    exportAlighmentNode->addChild(exportMenu);
    
    auto exportBG = CCScale9Sprite::create("geode.loader/black-square.png", {0,0, 40, 40});
    exportBG->setPosition({-180, 53});
    exportBG->setContentSize({71, 81});
    exportBG->setScale(1.2f);
    exportBG->setOpacity(150); 
    exportAlighmentNode->addChild(exportBG);

    auto exportLabel = CCLabelBMFont::create("Export", "bigFont.fnt");
    exportLabel->setPosition({-180, 92});
    exportLabel->setScale(0.575f);
    exportLabel->setZOrder(1);
    exportAlighmentNode->addChild(exportLabel);

    auto selectedS = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    selectedS->setScale(0.425f);
    auto deselectedS = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    deselectedS->setScale(0.425f);

    exportWithLabels = CCMenuItemToggler::create(deselectedS, selectedS, this, menu_selector(DTExportImportLayer::onExportWithLabelsClicked));
    exportWithLabels->setPosition({-29, 23});
    exportWithLabels->toggle(Save::getExportWOutLabels());
    exportWithLabelsB = Save::getExportWOutLabels();
    exportMenu->addChild(exportWithLabels);

    auto exportWithLabelsLabel = CCLabelBMFont::create("Without Labels", "bigFont.fnt");
    exportWithLabelsLabel->setPosition({-170, 70});
    exportWithLabelsLabel->setScale(0.225f);
    exportWithLabelsLabel->setZOrder(1);
    exportAlighmentNode->addChild(exportWithLabelsLabel);

    auto exportBS = CCSprite::createWithSpriteFrameName("GJ_shareBtn_001.png");
    exportBS->setScale(0.6f);
    auto exportButton = CCMenuItemSpriteExtra::create(
        exportBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onExportClicked)
    );
    exportButton->setPosition({-15, -13});
    exportMenu->addChild(exportButton);

    auto exportFolderBS = CCSprite::createWithSpriteFrameName("accountBtn_myLevels_001.png");
    exportFolderBS->setScale(0.625f);
    auto exportFolderButton = CCMenuItemSpriteExtra::create(
        exportFolderBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onOpenExportsFolderClicked)
    );
    exportFolderButton->setPosition({25, -12});
    exportMenu->addChild(exportFolderButton);

    //import

    auto importAlighmentNode = CCNode::create();
    importAlighmentNode->setPosition({228, -54});
    AlighmentNode->addChild(importAlighmentNode);

    auto importMenu = CCMenu::create();
    importMenu->setPosition({-180, 47});
    importMenu->setZOrder(1);
    importAlighmentNode->addChild(importMenu);

    auto importBG = CCScale9Sprite::create("geode.loader/black-square.png", {0,0, 40, 40});
    importBG->setPosition({-180, 53});
    importBG->setContentSize({71, 81});
    importBG->setScale(1.2f);
    importBG->setOpacity(150); 
    importAlighmentNode->addChild(importBG);

    auto importLabel = CCLabelBMFont::create("import", "bigFont.fnt");
    importLabel->setPosition({-180, 92});
    importLabel->setScale(0.575f);
    importLabel->setZOrder(1);
    importAlighmentNode->addChild(importLabel);

    auto importWarningLabel = CCLabelBMFont::create("might not be\naccurate!", "bigFont.fnt");
    importWarningLabel->setPosition({-180, 67});
    importWarningLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    importWarningLabel->setColor({228, 128, 128});
    importWarningLabel->setScale(0.35f);
    importWarningLabel->setZOrder(1);
    importAlighmentNode->addChild(importWarningLabel);

    auto importBS = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
    auto importBButton = CCMenuItemSpriteExtra::create(
        importBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onImportClicked)
    );
    importBButton->setPosition({0, -17});
    importMenu->addChild(importBButton);

    loading = LoadingCircle::create();
    loading->setVisible(false);
    m_mainLayer->addChild(loading);
    loading->show();

    //info

    auto ExportInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    ExportInfoBS->setScale(0.55f);
    auto ExportInfoButton = CCMenuItemSpriteExtra::create(
        ExportInfoBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onExportInfo)
    );
    ExportInfoButton->setPosition({41,53});
    exportMenu->addChild(ExportInfoButton);

    auto ImportInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    ImportInfoBS->setScale(0.55f);
    auto ImportInfoButton = CCMenuItemSpriteExtra::create(
        ImportInfoBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onImportInfo)
    );
    ImportInfoButton->setPosition({41,53});
    importMenu->addChild(ImportInfoButton);

    return true;
}

void DTExportImportLayer::onExportWithLabelsClicked(CCObject*){
    exportWithLabelsB = !exportWithLabelsB;
    Save::setExportWOutLabels(exportWithLabelsB);
}

void DTExportImportLayer::onExportClicked(CCObject*){
    auto DPath = Mod::get()->getSaveDir() / "exports";

    auto _ = file::createDirectory(DPath);

    DPath = DPath / (m_DTLayer->m_MyLevelStats.levelName + ".txt");

    file::FilePickOptions options;
    options.defaultPath = DPath;
    file::FilePickOptions::Filter filterlol;
    filterlol.description = "select export location.";
    std::vector<file::FilePickOptions::Filter> filters = {filterlol};
    options.filters = filters;

    file::pick(file::PickMode::SaveFile, options).listen([this](Result<std::filesystem::path>* path) {
            if (path->isOk()) {
                std::string ToWrite = "";

                if (Save::getExportWOutLabels()){
                    std::string s = m_DTLayer->deathsString;
                    for (int i = 0; i < s.length(); i++)
                    {
                        if (StatsManager::ContainsAtIndex(i, "<nbc>", s) || StatsManager::ContainsAtIndex(i, "<sbc>", s)){
                            s = s.erase(i, 5);
                        }
                    }
                    s += "\n\n" + m_DTLayer->RunString;
                    ToWrite += s;
                }
                else{
                    std::map<int, std::string> text;
                    auto currLayout = Save::getLayout();
                    for (int i = 0; i < currLayout.size(); i++)
                    {
                        std::string s = m_DTLayer->modifyString(currLayout[i].text);
                        for (int i = 0; i < s.length(); i++)
                        {
                            if (StatsManager::ContainsAtIndex(i, "<nbc>", s) || StatsManager::ContainsAtIndex(i, "<sbc>", s)){
                                s = s.erase(i, 5);
                                
                            }
                        }

                        text[currLayout[i].line] += s + "\n";
                    }

                    for (auto t : text)
                    {
                        ToWrite += t.second;
                        ToWrite += "\n";
                    }
                    ToWrite = ToWrite.substr(0, ToWrite.size() - 1);
                    
                }

                auto currPath = path->unwrap();

                if (currPath.extension() != ".txt")
                    currPath.replace_extension(".txt");


                auto _ = file::writeString(currPath, ToWrite);

                FLAlertLayer::create("Success!", fmt::format("<cy>Your text file {} has been created!</c>\n\n Location: {}", currPath.stem().string(), currPath.string()), "OK")->show();
            }
        },
        [] (auto progress) {},
        [] () {}
    );
}

void DTExportImportLayer::onOpenExportsFolderClicked(CCObject*){
    auto DPath = Mod::get()->getSaveDir() / "exports";

    auto _ = file::createDirectory(DPath);

    file::openFolder(DPath);
}

void DTExportImportLayer::onImportClicked(CCObject*){
    if (importing) return;
    importing = true;
    auto DPath = Mod::get()->getSaveDir() / "exports";

    auto _ = file::createDirectory(DPath);

    file::FilePickOptions options;
    options.defaultPath = DPath;
    file::FilePickOptions::Filter filterlol;
    filterlol.description = "select file to import.";
    std::vector<file::FilePickOptions::Filter> filters = {filterlol};
    options.filters = filters;

    loading->setVisible(true);

    openFileLocListener.bind([this](Task<Result<std::filesystem::path>>::Event* e){
        if (auto* pathPacked = e->getValue()){
            if (pathPacked->isOk()) {
                std::filesystem::path path = pathPacked->unwrap();

                auto fileTextRes = file::readString(path);
                if (fileTextRes.isOk()){
                    std::string fileText = fileTextRes.value();

                    

                    Deaths deathDetected{};
                    Runs runsDetected{};

                    int searchPhase = 0;
                    std::string num = "";
                    std::string num2 = "";

                    bool isRun = false;

                    std::string amount = "";

                    for (int i = 0; i < fileText.length(); i++)
                    {
                        //log::info("before 1 | num {} | num2 {} | isRun {} | amount {} | searchPhase {} | char '{}'", num, num2, isRun, amount, searchPhase, fileText[i]);

                        if (std::isdigit(fileText[i]) && (searchPhase == 0 || searchPhase == 1)){
                            if (searchPhase == 0)
                                searchPhase = 1;
                            if (!isRun)
                                num += fileText[i];
                            else
                                num2 += fileText[i];
                        }
                        else if (fileText[i] == '-' && searchPhase == 1){
                            isRun = true;
                        }
                        else if (!std::isdigit(fileText[i]) && fileText[i] != ' ' && fileText[i] != '%' && searchPhase == 1 && fileText[i] != '-'){
                            searchPhase = 2;
                        }
                        else if (searchPhase == 0 || searchPhase == 1 && fileText[i] != ' ' && fileText[i] != '%') {
                            searchPhase = 0;
                            num = "";
                            isRun = false;
                        }

                        if (searchPhase == 3 && std::isdigit(fileText[i])){
                            amount += fileText[i];
                        }
                        else if (fileText[i] != ' ' && searchPhase == 3){
                            searchPhase = 4;
                        }

                        //log::info("before 2 | num {} | num2 {} | isRun {} | amount {} | searchPhase {} | char '{}'", num, num2, isRun, amount, searchPhase, fileText[i]);

                        if (searchPhase == 2 && fileText[i] != '\r' && fileText[i] != '\n'){

                            if (tolower(fileText[i]) != 'x' && fileText[i] != ' '){
                                num = "";
                                num2 = "";
                                isRun = false;
                                amount = "";
                                searchPhase = 0;
                            }
                            else if (tolower(fileText[i]) == 'x'){
                                searchPhase = 3;
                            }

                        }

                        //log::info("before 3 | num {} | num2 {} | isRun {} | amount {} | searchPhase {} | char '{}'", num, num2, isRun, amount, searchPhase, fileText[i]);

                        if (fileText[i] == '\n' || fileText[i] == ',' || fileText.length() - 1 == i){
                            if (num != "" && amount == ""){
                                amount = "1";
                            }

                            if (searchPhase >= 1 && num != ""){
                                if (!isRun)
                                    deathDetected[num] += std::stoi(amount);
                                else
                                    runsDetected[fmt::format("{}-{}", num, num2)] += std::stoi(amount);
                            }

                            num = "";
                            num2 = "";
                            isRun = false;
                            amount = "";
                            searchPhase = 0;
                        }

                        //log::info("num {} | num2 {} | isRun {} | amount {} | searchPhase {} | char '{}'", num, num2, isRun, amount, searchPhase, fileText[i]);
                    }

                    loading->setVisible(false);
                    importing = false;

                    auto CILayer = confirmImportLayer::create(m_DTLayer, deathDetected, runsDetected);
                    CILayer->setZOrder(100);
                    this->addChild(CILayer);
                }
                else{
                    geode::Notification::create("Error reading file! :(", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
                    loading->setVisible(false);
                    importing = false;
                }
            }
            else{
                importing = false;
                loading->setVisible(false);
                geode::Notification::create("Failed opening the file!", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
            }
        }
    });

    auto task = file::pick(file::PickMode::OpenFile, options);

    openFileLocListener.setFilter(task);
}

void DTExportImportLayer::onExportInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "Click the big button to export your runs onto a .txt file!\nThe smaller folder button will send you to the exports folder.", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}

void DTExportImportLayer::onImportInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "This allows you to import runs from old .txt files that you were tracking runs on right onto death tracker easiely!\n\n(This might not be accurate in some cases)", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}
