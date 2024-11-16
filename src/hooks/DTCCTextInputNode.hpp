#include <Geode/modify/CCTextInputNode.hpp>

using namespace geode::prelude;

class $modify(DTCCTextInputNode, CCTextInputNode) {
    struct Fields{
        char const* lastText;

        SEL_CallFunc callback;
        CCObject* target = nullptr;
    };

    public:
        void setCallback(const SEL_CallFunc& _callback, CCObject* const& _target);

        bool onTextFieldInsertText(cocos2d::CCTextFieldTTF* pSender, char const* text, int nLen, cocos2d::enumKeyCodes keyCodes);
};