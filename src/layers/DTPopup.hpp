#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

class DTPopup : public Popup<FLAlertLayer* const&, GJGameLevel* const&> {
    protected:
        bool setup(FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level) override;

        void onClose(cocos2d::CCObject*);

        FLAlertLayer* m_InfoAlertLayer;

        GJGameLevel* m_Level;

        SimpleTextArea* m_DeathsLabel;
        ScrollLayer* m_SLayer;

        LevelStats m_MyLevelStats;
        bool m_noSavedData;

        CCSprite* m_SessionsButtonIconInactive;
        CCSprite* m_SessionsButtonIconActive;

        CCSprite*  m_passRateButtonIconInactive;
        CCSprite*  m_passRateButtonIconActive;

        std::vector<std::pair<std::string, float>> m_DeathStrings;
        std::vector<std::string> m_RunStrings;

        std::vector<std::pair<std::string, float>> m_SessionStrings;
        std::vector<std::string> m_SessionRunStrings;

        void update(float delta);
    public:
        static DTPopup* create(float width, float hight, FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level);

        std::vector<std::pair<std::string, float>> CreateDeathsString(Deaths deaths, NewBests newBests, std::string NewBestsColorString);
        std::vector<std::string> CreateRunsString(Runs runs);

        void CopyText(CCObject* sender);

        void TogglePassRate(CCObject* sender);
        void ToggleSessions(CCObject* sender);

        enum texts {deaths, DeathsPassRate, Sessions, SessionsPassRate};
        void refreshText(texts textID);

        texts m_CurrentPage;

        void ShowInfo(CCObject* sender);
};