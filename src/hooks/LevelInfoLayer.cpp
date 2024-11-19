#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Settings.hpp"
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class $modify(myLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* btn = nullptr;
    };
    
    bool init(GJGameLevel* p0, bool p1){
        if (!LevelInfoLayer::init(p0, p1)) return false;

        StatsManager::loadLevelStats(p0);


        auto s = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
        auto s2 = CCSprite::createWithSpriteFrameName("miniSkull_001.png");
        s2->setPosition(s->getContentSize() / 2);
        s->addChild(s2);
        if (!Settings::getLeftMenuEnabled())
            s->setScale(0.75f);
        m_fields->btn = CCMenuItemSpriteExtra::create(
            s,
            nullptr,
            this,
            menu_selector(myLevelInfoLayer::openDTLayer)
        );
        m_fields->btn->setID("dt-skull-button");
        m_fields->btn->setZOrder(1);
        m_fields->btn->setVisible(false);

        if (Settings::getLeftMenuEnabled()){
            auto leftSideMenu = getChildByID("left-side-menu");
            leftSideMenu->addChild(m_fields->btn);
            leftSideMenu->updateLayout();
        }
        else{
            auto otherMenu = getChildByID("other-menu");
            auto settingsMenu = getChildByID("settings-menu");

            otherMenu->addChild(m_fields->btn);
            if (otherMenu->getChildByID("favorite-button")->isVisible())
                m_fields->btn->setPosition({otherMenu->getChildByID("favorite-button")->getPositionX(), settingsMenu->getChildByID("settings-button")->getPositionY()});
            else
                m_fields->btn->setPosition({otherMenu->getChildByID("favorite-button")->getPosition()});
            otherMenu->updateLayout();
        }

        

        auto stats = StatsManager::getLevelStats(p0);
        if (stats.currentBest != -1){
            stats.attempts = p0->m_attempts;
            stats.levelName = p0->m_levelName;
            stats.difficulty = StatsManager::getDifficulty(p0);

            StatsManager::saveData(stats, p0);
            StatsManager::saveBackup(stats, p0);
        }
        else{
            Notification::create("Failed to load Deaths json.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
        }

        schedule(schedule_selector(myLevelInfoLayer::checkIfPlayVisible));

        return true;
    }

    void checkIfPlayVisible(float delta){
        if (this->m_playBtnMenu->isVisible()){
            m_fields->btn->setVisible(true);

            auto stats = StatsManager::getLevelStats(this->m_level);
            if (stats.currentBest != -1){
                stats.difficulty = StatsManager::getDifficulty(this->m_level);

                StatsManager::saveData(stats, this->m_level);
            }
            unschedule(schedule_selector(myLevelInfoLayer::checkIfPlayVisible));
        }
    }

    void openDTLayer(CCObject*){
        auto dtLayer = DTLayer::create(this->m_level);
        this->addChild(dtLayer);
    }   
};