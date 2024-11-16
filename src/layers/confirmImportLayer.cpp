#include "../layers/confirmImportLayer.hpp"
#include "../utils/Save.hpp"
#include "Geode/ui/TextArea.hpp"

confirmImportLayer* confirmImportLayer::create(DTLayer* const& layer, const Deaths& ds, const Runs& rs) {
    auto ret = new confirmImportLayer();
    if (ret && ret->initAnchored(270, 230, layer, ds, rs, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool confirmImportLayer::setup(DTLayer* const& layer, const Deaths& ds, const Runs& rs) {

    m_DTLayer = layer;
    deaths = ds;
    runs = rs;

    auto AlignmentNode = CCNode::create();
    AlignmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(AlignmentNode);

    auto prevewCont = CCNode::create();
    prevewCont->setPositionX(-46);
    AlignmentNode->addChild(prevewCont);

    auto exportBG = CCScale9Sprite::create("geode.loader/black-square.png", {0,0, 40, 40});
    exportBG->setContentSize({135, 195});
    exportBG->setOpacity(150);
    prevewCont->addChild(exportBG);

    auto sl = ScrollLayer::create({40, 40});
    sl->setContentSize({135, 195});
    sl->setPosition(-sl->getContentSize() / 2);
    sl->setZOrder(1);
    prevewCont->addChild(sl);

    std::string sToShow = "";

    for (auto d : ds){
        sToShow += d.first + "% x" + std::to_string(d.second) + "\n";
    }

    sToShow += "-\n ";

    for (auto r : rs){
        auto run = StatsManager::splitRunKey(r.first);

        sToShow += std::to_string(run.start) + "%-" + std::to_string(run.end) + "% x" + std::to_string(r.second) + "\n";
    }

    auto TextPrev = SimpleTextArea::create(sToShow, "chatFont.fnt", 0.75f);
    TextPrev->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    TextPrev->setPositionX(135 / 2);
    sl->m_contentLayer->addChild(TextPrev);

    float csize = 0;

    for (int i = 0; i < TextPrev->getLines().size(); i++)
    {
        csize += TextPrev->getLines()[i]->getScaledContentSize().height;
    }

    if (csize > 195){
        sl->m_contentLayer->setContentSize({135, csize});
        TextPrev->setPositionY(csize);
    }
    else{
        sl->m_contentLayer->setContentSize({135, 195});
        TextPrev->setPositionY(195);
    }

    text = TextPrev;

    auto infoText = SimpleTextArea::create("Are the runs\ndetected\naccurate?", "bigFont.fnt", 0.425f);
    infoText->setPosition({74, 76});
    infoText->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    AlignmentNode->addChild(infoText);

    auto infoText2 = SimpleTextArea::create("Doing this\nwill add\nthese runs\nonto your\nsave!", "bigFont.fnt", 0.5f);
    infoText2->setColor({ 255, 255, 0, 255 });
    infoText2->setPosition({73, 0});
    infoText2->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    AlignmentNode->addChild(infoText2);

    auto yesBS = ButtonSprite::create("Yes");
    yesBS->setScale(0.7f);
    auto yesButton = CCMenuItemSpriteExtra::create(
        yesBS,
        nullptr,
        this,
        menu_selector(confirmImportLayer::yesClicked)
    );
    yesButton->setPosition(ccp(51, -79) + m_size / 2);
    m_buttonMenu->addChild(yesButton);

    auto noBS = ButtonSprite::create("No");
    noBS->setScale(0.7f);
    auto noButton = CCMenuItemSpriteExtra::create(
        noBS,
        nullptr,
        this,
        menu_selector(confirmImportLayer::onClose)
    );
    noButton->setPosition(ccp(99, -79) + m_size / 2);
    m_buttonMenu->addChild(noButton);

    m_closeBtn->setVisible(false);

    sl->moveToTop();

    scheduleUpdate();

    return true;
}


void confirmImportLayer::update(float delta){
    text->setVisible(true);
}

void confirmImportLayer::yesClicked(CCObject*){
    m_DTLayer->m_MyLevelStats.deaths.insert(deaths.begin(), deaths.end());
    m_DTLayer->m_MyLevelStats.runs.insert(runs.begin(), runs.end());

    StatsManager::saveData(m_DTLayer->m_MyLevelStats, m_DTLayer->m_Level);
    m_DTLayer->UpdateSharedStats();
    m_DTLayer->refreshStrings();
    m_DTLayer->RefreshText();

    geode::Notification::create("Applied changes!", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();

    onClose(nullptr);
}