#include "../hooks/DTCCTextInputNode.hpp"

void DTCCTextInputNode::setCallback(SEL_CallFunc _callback, CCObject* _target){
    m_fields->callback = _callback;
    m_fields->target = _target;
}

bool DTCCTextInputNode::onTextFieldInsertText(cocos2d::CCTextFieldTTF* pSender, char const* text, int nLen, cocos2d::enumKeyCodes keyCodes){
    m_fields->lastText = text;

    if (m_fields->target)
        this->runAction(CCCallFunc::create(m_fields->target, m_fields->callback));
    
    return CCTextInputNode::onTextFieldInsertText(pSender, text, nLen, keyCodes);
}