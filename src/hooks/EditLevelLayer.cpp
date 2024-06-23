#include <Geode/modify/EditLevelLayer.hpp>
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

class $modify(DTEditLevelLayer, EditLevelLayer) {

    bool init(GJGameLevel* level){
        if (!EditLevelLayer::init(level)) return false;
        StatsManager::loadLevelStats(level);

        auto IBMenu = getChildByID("info-button-menu");

        auto s = CCSprite::create("dt_skullBtn.png"_spr);
        s->setScale(0.25f);
        auto btn = CCMenuItemSpriteExtra::create(
            s,
            nullptr,
            this,
            menu_selector(DTEditLevelLayer::openDTLayer)
        );
        btn->setZOrder(1);
        btn->setID("death-tracker-menu"_spr);

        IBMenu->addChild(btn);
        btn->setPosition({IBMenu->getChildByID("info-button")->getPosition() + ccp(btn->getScaledContentSize().width, 0)});
        IBMenu->updateLayout();

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

    void openDTLayer(CCObject*){
        auto dtLayer = DTLayer::create(this->m_level);
        this->addChild(dtLayer);
    }
    
};
