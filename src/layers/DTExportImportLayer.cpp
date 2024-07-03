#include "../layers/DTExportImportLayer.hpp"
#include "../utils/Save.hpp"

DTExportImportLayer* DTExportImportLayer::create(DTLayer* const& layer) {
    auto ret = new DTExportImportLayer();
    if (ret && ret->init(480, 240, layer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DTExportImportLayer::setup(DTLayer* const& layer) {

    m_DTLayer = layer;

    auto AlighmentNode = CCNode::create();
    AlighmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(AlighmentNode);

    auto exportAlighmentNode = CCNode::create();
    AlighmentNode->addChild(exportAlighmentNode);

    auto exportMenu = CCMenu::create();
    exportMenu->setPosition({-180, 47});
    exportMenu->setZOrder(1);
    exportAlighmentNode->addChild(exportMenu);

    //export
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

    auto exportBS = ButtonSprite::create("Export");
    exportBS->setScale(0.65f);
    auto exportButton = CCMenuItemSpriteExtra::create(
        exportBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onExportClicked)
    );
    exportButton->setPosition({0, 2});
    exportMenu->addChild(exportButton);

    auto exportFolderBS = CCSprite::createWithSpriteFrameName("accountBtn_myLevels_001.png");
    exportFolderBS->setScale(0.625f);
    auto exportFolderButton = CCMenuItemSpriteExtra::create(
        exportFolderBS,
        nullptr,
        this,
        menu_selector(DTExportImportLayer::onOpenExportsFolderClicked)
    );
    exportFolderButton->setPosition({0, -25});
    exportMenu->addChild(exportFolderButton);

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
