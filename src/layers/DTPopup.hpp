#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

class DTPopup : public Popup<FLAlertLayer* const&, GJGameLevel* const&> {
    protected:
        bool setup(FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level) override;

        void onClose(cocos2d::CCObject*) override;

        FLAlertLayer* m_InfoAlertLayer;

        GJGameLevel* m_Level;

        SimpleTextArea* m_DeathsLabel;
        ScrollLayer* m_SLayer;
        CCNode* m_PlatformerInfoCont;

        LevelStats m_MyLevelStats;
        bool m_noSavedData;

        CCSprite* m_SessionsButtonIconInactive;
        CCSprite* m_SessionsButtonIconActive;

        CCSprite*  m_passRateButtonIconInactive;
        CCSprite*  m_passRateButtonIconActive;

        std::vector<std::tuple<std::string, int, float>> m_DeathStrings;
        std::vector<std::tuple<std::string, int>> m_RunStrings;

        std::vector<std::tuple<std::string, int, float>> m_SessionStrings;
        std::vector<std::tuple<std::string, int>> m_SessionRunStrings;

        void update(float delta) override;
    public:
        static DTPopup* create(float width, float hight, FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level);

        std::vector<std::tuple<std::string, int, float>> CreateDeathsString(Deaths deaths, NewBests newBests, std::string NewBestsColorString);
        std::vector<std::tuple<std::string, int>> CreateRunsString(Runs runs);

        void CopyText(CCObject* sender);

        void TogglePassRate(CCObject* sender);
        void ToggleSessions(CCObject* sender);

        enum texts {deaths, DeathsPassRate, Sessions, SessionsPassRate};
        void refreshText(texts textID);
        void refreshPlatformerRuns(texts textID);

        texts m_CurrentPage;

        void ShowInfo(CCObject* sender);

        CCNode* CreateGraph(std::vector<std::tuple<std::string, int, float>> deathsString, float bestRun, CCPoint Scaling = {1, 1});
        float GetBestRun(NewBests bests);

        CCNode* createPlatformerDeath(std::string deathP, int Count, float passRate = -1);
        std::pair<CCNode*, float> createCheckpointLabel(std::string deathP);
};