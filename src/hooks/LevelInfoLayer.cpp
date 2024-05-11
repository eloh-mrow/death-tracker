#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

class $modify(myLevelInfoLayer, LevelInfoLayer) {
    CCMenuItemSpriteExtra* btn = nullptr;

    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelInfoLayer::onLevelInfo", INT64_MIN + 1);
    }

    void onLevelInfo(CCObject* sender) {
        DTPopupManager::setInfoAlertOpen(true);
        DTPopupManager::setCurrentLevel(m_level);
        LevelInfoLayer::onLevelInfo(sender);
    }

    bool init(GJGameLevel* p0, bool p1){
        if (!LevelInfoLayer::init(p0, p1)) return false;

        auto otherMenu = getChildByID("other-menu");
        auto settingsMenu = getChildByID("settings-menu");

        auto s = CCSprite::create("dt_skullBtn.png"_spr);
        s->setScale(0.25f);
        m_fields->btn = CCMenuItemSpriteExtra::create(
            s,
            nullptr,
            this,
            menu_selector(myLevelInfoLayer::openDTLayer)
        );
        m_fields->btn->setZOrder(1);
        m_fields->btn->setVisible(false);
        m_fields->btn->setID("death-tracker-menu"_spr);

        otherMenu->addChild(m_fields->btn);
        if (otherMenu->getChildByID("favorite-button")->isVisible())
            m_fields->btn->setPosition({otherMenu->getChildByID("favorite-button")->getPositionX(), settingsMenu->getChildByID("settings-button")->getPositionY()});
        else
            m_fields->btn->setPosition({otherMenu->getChildByID("favorite-button")->getPosition()});
        otherMenu->updateLayout();

        if (ghc::filesystem::exists(StatsManager::getLevelSaveFilePath(p0))){
                auto stats = StatsManager::getLevelStats(p0);

                stats.attempts = p0->m_attempts;
                stats.levelName = p0->m_levelName;
                stats.difficulty = StatsManager::getDifficulty(p0);

                StatsManager::saveData(stats, p0);
            }

        schedule(schedule_selector(myLevelInfoLayer::checkIfPlayVisible));

        return true;
    }

    void checkIfPlayVisible(float delta){
        if (this->m_playBtnMenu->isVisible()){
            m_fields->btn->setVisible(true);

            if (ghc::filesystem::exists(StatsManager::getLevelSaveFilePath(this->m_level))){
                auto stats = StatsManager::getLevelStats(this->m_level);

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