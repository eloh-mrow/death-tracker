#include "../hooks/DTEditLevelLayer.hpp"

bool DTEditLevelLayer::init(GJGameLevel* level){
    if (!EditLevelLayer::init(level)) return false;
    StatsManager::loadLevelStats(level);

    auto s = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
    auto s2 = CCSprite::createWithSpriteFrameName("miniSkull_001.png");
    s2->setPosition(s->getContentSize() / 2);
    s->addChild(s2);
    s->setScale(0.75f);
    if (Settings::getLeftMenuEnabled())
        s->setScale(0.8f);
    auto btn = CCMenuItemSpriteExtra::create(
        s,
        nullptr,
        this,
        menu_selector(DTEditLevelLayer::openDTLayer)
    );
    btn->setID("dt-skull-button");
    btn->setZOrder(1);

    if (Settings::getLeftMenuEnabled()){
        auto folderMenu = getChildByID("folder-menu");
        folderMenu->addChild(btn);
        folderMenu->updateLayout();
    }
    else{
        auto IBMenu = getChildByID("info-button-menu");
        IBMenu->addChild(btn);
        btn->setPosition({IBMenu->getChildByID("info-button")->getPosition() + ccp(btn->getScaledContentSize().width, 0)});
        IBMenu->updateLayout();
    }
    

    auto stats = StatsManager::getLevelStats(level);
    if (stats.currentBest != -1){
        stats.attempts = level->m_attempts;
        stats.levelName = level->m_levelName;
        stats.difficulty = StatsManager::getDifficulty(level);

        StatsManager::saveData(stats, level);
        StatsManager::saveBackup(stats, level);
    }
    else{
        Notification::create("Failed to load Deaths json.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
    }

    return true;
}

void DTEditLevelLayer::openDTLayer(CCObject*){
    auto dtLayer = DTLayer::create(this->m_level);
    this->addChild(dtLayer);
}

