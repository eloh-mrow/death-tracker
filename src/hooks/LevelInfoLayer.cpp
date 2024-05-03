#include <Geode/modify/LevelInfoLayer.hpp>
#include "../managers/DTPopupManager.hpp"
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

class $modify(myLevelInfoLayer, LevelInfoLayer) {
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

        auto s = CCSprite::create("dt_skullBtn.png"_spr);
        s->setScale(.2f);
        auto btn = CCMenuItemSpriteExtra::create(
            s,
            this,
            menu_selector(myLevelInfoLayer::openDTLayer)
        );
        btn->setZOrder(1);
        btn->setID("death-tracker-menu"_spr);

        otherMenu->addChild(btn);
        btn->setPositionY(12.5f);
        otherMenu->getChildByID("info-button")->setPositionY(-12.5f);
        otherMenu->updateLayout();

        if (ghc::filesystem::exists(StatsManager::getLevelSaveFilePath(p0))){
            auto stats = StatsManager::getLevelStats(p0);

            stats.attempts = p0->m_attempts;
            stats.levelName = p0->m_levelName;
            stats.difficulty = StatsManager::getDifficulty(p0);

            StatsManager::saveData(stats, p0);
        }

        return true;
    }

    void openDTLayer(CCObject*){
        auto dtLayer = DTLayer::create(this->m_level);
        this->addChild(dtLayer);
    }   
};