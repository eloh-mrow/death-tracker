#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../managers/StatsManager.hpp"
#include "../utils/Settings.hpp"

using namespace geode::prelude;

long long getNowSeconds() {
    using namespace std::chrono;
    auto now = system_clock::now();
    return time_point_cast<seconds>(now).time_since_epoch().count();
}

class $modify(DTPlayLayer, PlayLayer) {
    Run currentRun;

    bool disableCompletedLevelTracking() {
        return Settings::isCompletedLevelTrackingDisabled()
            && m_level->m_newNormalPercent2.value() == 100
            && m_level->m_levelType == GJLevelType::Saved;
    }

    /* hooks
    ========== */
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        log::info("PlayLayer::init()");

        StatsManager::loadLevelStats(level);
        auto levelKey = StatsManager::getLevelKey(level);
        auto sessionLength = Settings::getMaxSessionLength();

        // schedule create a new session
        // based on the session length setting
        switch (sessionLength) {
            // reset session per level play
            case -2: {
                StatsManager::scheduleCreateNewSession(true);
                break;
            }

            // reset session per game launch
            case -1: {
                // returns true if a new session
                // was created during this game launch
                if (StatsManager::hasPlayedLevel()) break;

                StatsManager::scheduleCreateNewSession(true);
                break;
            }

            // reset session based on session.lastPlayed
            default: {
                auto session = StatsManager::getSession();
                auto now = getNowSeconds();

                // reset if the time since lastPlayed
                // is longer than the session length
                if (now - session.lastPlayed > sessionLength)
                    StatsManager::scheduleCreateNewSession(true);

                break;
            }
        }

        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        log::info("PlayLayer::resetLevel()");
        m_fields->currentRun.start = this->getCurrentPercent();
    }

    void destroyPlayer(PlayerObject* player, GameObject* p1) {
        PlayLayer::destroyPlayer(player, p1);
        if (m_level->isPlatformer()) return;
        if (!player->m_isDead) return;
        if (player != m_player1) return;

        // disable tracking deaths on completed levels
        if (DTPlayLayer::disableCompletedLevelTracking()) return;
        log::info("PlayLayer::destroyPlayer()");

        m_fields->currentRun.end = this->getCurrentPercent();

        // log deaths from 0 in normal mode
        if (m_fields->currentRun.start == 0 && !m_isPracticeMode)
            StatsManager::logDeath(m_fields->currentRun.end);

        // anything else is a run
        else StatsManager::logRun(m_fields->currentRun);
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        if (m_level->isPlatformer()) return;

        // disable tracking deaths on completed levels
        if (DTPlayLayer::disableCompletedLevelTracking()) return;
        log::info("PlayLayer::levelComplete()");

        m_fields->currentRun.end = this->getCurrentPercent();
        StatsManager::logRun(m_fields->currentRun);
    }

    void onQuit() {
        PlayLayer::onQuit();
        log::info("PlayLayer::onQuit()");

        // schedule session gets reset
        // if they back out before dying
        // this cancels creating a new session
        StatsManager::scheduleCreateNewSession(false);

        // set session.lastPlayed
        auto sessionLength = Settings::getMaxSessionLength();
        if (sessionLength < 0) return;

        StatsManager::setSessionLastPlayed(getNowSeconds());
    }
};