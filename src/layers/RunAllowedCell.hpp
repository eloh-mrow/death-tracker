#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class RunAllowedCell : public CCMenuItemSpriteExtra {
protected:
    bool init(int Precent, float scale, CCNode* DTLayer, CCNode* DTSSLayer);
public:
    static RunAllowedCell* create(int Precent, float scale, CCNode* DTLayer, CCNode* DTSSLayer);

    int m_Precent;
    CCNode* m_DTLayer;
    CCNode* m_DTSSLayer;
    ButtonSprite* buttonSprite;

    void DeleteMe(CCObject*);
};