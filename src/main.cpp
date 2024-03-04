#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
#include <managers/StatsManager.hpp>

class $modify(PlayLayer){

    Run currentRun;

    void startGame(){
        PlayLayer::startGame();
        m_fields->currentRun.start = this->getCurrentPercent();
    }

    void resetLevel(){
        PlayLayer::resetLevel();
        m_fields->currentRun.start = this->getCurrentPercent();
    }

    void destroyPlayer(PlayerObject* player, GameObject* p1){
        PlayLayer::destroyPlayer(player, p1);
        if (m_level->isPlatformer() && !player->m_isDead && player != m_player1) return;

        m_fields->currentRun.end = this->getCurrentPercent();

        if (m_fields->currentRun.start == 0 && !m_isPracticeMode)
            StatsManager::logDeath(m_fields->currentRun.end);
        else 
            StatsManager::logRun(m_fields->currentRun);
    }

    void levelComplete(){
        PlayLayer::levelComplete();
        if (m_level->isPlatformer()) return;

        m_fields->currentRun.end = this->getCurrentPercent();
        StatsManager::logRun(m_fields->currentRun);
    }
};