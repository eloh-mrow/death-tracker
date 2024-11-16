#include <Geode/modify/ColorSelectPopup.hpp>

using namespace geode::prelude;

class $modify(DTColorSelectPopup, ColorSelectPopup) {
    struct Fields{
        SEL_CallFunc callback;
        CCObject* target = nullptr;
    };

    public:
        void setCallback(const SEL_CallFunc& _callback, CCObject* const& _target);

        void colorValueChanged(cocos2d::ccColor3B newColor);
};