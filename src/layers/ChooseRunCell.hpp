#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class ChooseRunCell : public CCNode {
protected:
    bool init(int Precent, CCNode* DTGLayer);
public:
    static ChooseRunCell* create(int Precent, CCNode* DTLayer);

    int m_Precent;
    CCNode* m_DTGraphLayer;

    void ChooseMe(CCObject*);
};