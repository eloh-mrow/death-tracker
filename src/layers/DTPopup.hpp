#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"

using namespace geode::prelude;

class DTPopup : public Popup<FLAlertLayer* const&, GJGameLevel* const&> {
    protected:
        bool setup(FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level) override;

        void onClose(cocos2d::CCObject*);

        FLAlertLayer* m_InfoAlertLayer;

        GJGameLevel* m_Level;

        std::vector<SimpleTextArea*> m_TextAreas;

        void update(float delta);
    public:
        static DTPopup* create(float width, float hight, FLAlertLayer* const& InfoAlertLayer, GJGameLevel* const& Level);

        std::pair<std::string, std::string> CreateDeathsStrings();
};