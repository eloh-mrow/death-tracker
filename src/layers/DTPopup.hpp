#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTPopup : public Popup<FLAlertLayer* const&> {
    protected:
        bool setup(FLAlertLayer* const& InfoAlertLayer) override;

        void onClose(cocos2d::CCObject*);

        FLAlertLayer* m_InfoAlertLayer;
    public:
        static DTPopup* create(float width, float hight, FLAlertLayer* const& InfoAlertLayer);
};