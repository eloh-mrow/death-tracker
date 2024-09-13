#include <Geode/modify/ColorSelectPopup.hpp>

using namespace geode::prelude;

class $modify(DTColorSelectPopup, ColorSelectPopup) {
    struct Fields{
        SEL_CallFunc callback;
        CCObject* target = nullptr;
    };

    public:
        void setCallback(SEL_CallFunc _callback, CCObject* _target);

        void colorValueChanged(cocos2d::ccColor3B newColor);
};