#include <Geode/modify/PlayLayer.hpp>
#include "../managers/StatsManager.hpp"
#include "../utils/Settings.hpp"

using namespace geode::prelude;

class $modify(DTPlayLayer, PlayLayer) {
    Session* prevSession = nullptr;
    bool isSessionExpired = false;
    bool hasRespawned = false;
    Run currentRun;

    bool disableCompletedLevelTracking() {
        return Settings::isCompletedLevelTrackingDisabled()
            && m_level->m_newNormalPercent2.value() == 100
            && m_level->m_levelType == GJLevelType::Saved;
    }

    void updateSessionLastPlayed() {
        auto session = StatsManager::getSession();

        log::info("DTPlayLayer::updateSessionLastPlayed()\nisPrevSession = {}\nisSessionExpired = {}",
            session == m_fields->prevSession,
            m_fields->isSessionExpired
        );

        if (session == m_fields->prevSession && m_fields->isSessionExpired) return;
        StatsManager::updateSessionLastPlayed(true);
    }

    /* hooks
    ========== */
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("PlayLayer::levelComplete", -9999);
    }

    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        log::info("PlayLayer::init()");

        StatsManager::loadLevelStats(level);
        auto session = StatsManager::getSession();
        auto levelKey = StatsManager::getLevelKey(level);
        auto sessionLength = Settings::getMaxSessionLength();

        m_fields->prevSession = session;

        // schedule create a new session
        // based on the session length setting
        // -2 means default session (no previous session)
        if (session->lastPlayed != -2) {
            switch (sessionLength) {
                // reset session per level play
                case -2: {
                    m_fields->isSessionExpired = true;
                    StatsManager::scheduleCreateNewSession(true);
                    break;
                }

                // reset session per game launch
                case -1: {
                    // returns true if a new session
                    // was created during this game launch
                    if (StatsManager::hasPlayedLevel()) break;

                    m_fields->isSessionExpired = true;
                    StatsManager::scheduleCreateNewSession(true);
                    break;
                }

                // reset session based on session.lastPlayed
                default: {
                    auto now = StatsManager::getNowSeconds();

                    // reset if the time since lastPlayed
                    // is longer than the session length
                    if (now - session->lastPlayed > sessionLength) {
                        m_fields->isSessionExpired = true;
                        StatsManager::scheduleCreateNewSession(true);
                    }

                    break;
                }
            }
        }

        DTPlayLayer::updateSessionLastPlayed();
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        log::info("PlayLayer::resetLevel()");

        m_fields->hasRespawned = true;

        if (!m_level->isPlatformer())
            m_fields->currentRun.start = this->getCurrentPercentInt();
    }

    void destroyPlayer(PlayerObject* player, GameObject* p1) {
        PlayLayer::destroyPlayer(player, p1);

        if (!player->m_isDead) return;
        if (player != m_player1) return;

        // just in case some mod accidentally calls
        // PlayLayer::destroyPlayer() twice
        if (!m_fields->hasRespawned) return;
        m_fields->hasRespawned = false;

        // disable tracking deaths on completed levels
        if (DTPlayLayer::disableCompletedLevelTracking()) return;

        if (!m_level->isPlatformer())
            m_fields->currentRun.end = this->getCurrentPercentInt();

        log::info("PlayLayer::destroyPlayer()\ncurrentRun.start = {}\ncurrentRun.end = {}\nplatformer = {}",
            m_fields->currentRun.start,
            m_fields->currentRun.end,
            m_level->isPlatformer()
        );

        // log deaths from 0 in normal mode
        if (m_fields->currentRun.start == 0 && !m_isPracticeMode)
            StatsManager::logDeath(m_fields->currentRun.end);

        // anything else is a run
        // platformer runs only from 0
        else if (!m_level->isPlatformer() || m_fields->currentRun.start == 0)
            StatsManager::logRun(m_fields->currentRun);
    }

    void levelComplete() {
        PlayLayer::levelComplete();

        // same as PlayLayer::destroyPlayer()
        if (!m_fields->hasRespawned) return;
        m_fields->hasRespawned = false;

        // disable tracking deaths on completed levels
        if (DTPlayLayer::disableCompletedLevelTracking()) return;

        if (!m_level->isPlatformer())
            m_fields->currentRun.end = 100;

        log::info("PlayLayer::levelComplete()\ncurrentRun.start = {}\ncurrentRun.end = {}\nplatformer = {}",
            m_fields->currentRun.start,
            m_fields->currentRun.end,
            m_level->isPlatformer()
        );

        if (!m_level->isPlatformer() || m_fields->currentRun.start == 0)
            StatsManager::logRun(m_fields->currentRun);
    }

    void checkpointActivated(CheckpointGameObject* checkpt) {
        PlayLayer::checkpointActivated(checkpt);
        if (m_isPracticeMode) return;

        m_fields->currentRun.end++;
        log::info("PlayLayer::checkpointActivated() -- {}", m_fields->currentRun.end);
    }

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();
        log::info("PlayLayer::resetLevelFromStart()");

        m_fields->currentRun.end = 0;
    }

    void onQuit() {
        PlayLayer::onQuit();
        log::info("PlayLayer::onQuit()");

        // schedule session gets reset
        // this cancels creating a new session
        // if they back out before dying
        StatsManager::scheduleCreateNewSession(false);
        DTPlayLayer::updateSessionLastPlayed();
    }
};