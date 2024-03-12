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

        std::vector<SimpleTextArea*> m_TextAreas;

        LevelStats m_MyLevelStats;

        CCSprite* m_SessionsButtonIconInactive;
        CCSprite* m_SessionsButtonIconActive;

        CCSprite*  m_passRateButtonIconInactive;
        CCSprite*  m_passRateButtonIconActive;

        void update(float delta);
    public:
        static DTPopup* create(float width, float hight, FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level);

        std::pair<std::string, std::string> CreateDeathsStrings();

        void GetLevelStats();

        void CopyText(CCObject* sender);
};