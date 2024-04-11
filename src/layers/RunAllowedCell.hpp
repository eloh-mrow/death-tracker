#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class RunAllowedCell : public CCNode {
protected:
    bool init(int Precent, CCNode* DTLayer);
public:
    static RunAllowedCell* create(int Precent, CCNode* DTLayer);

    int m_Precent;
    CCNode* m_DTLayer;

    void DeleteMe(CCObject*);
};