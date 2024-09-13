#pragma once

#include <Geode/Geode.hpp>
#include "../managers/StatsManager.hpp"
#include "../layers/DTLinkLayer.hpp"

using namespace cocos2d;
using namespace geode;

class LinkLevelCell : public CCNode {
protected:
    bool init(CCNode* l, std::string levelKey, LevelStats stats, bool linked);
public:
    static LinkLevelCell* create(DTLinkLayer* DTL, std::string levelKey, LevelStats stats, bool linked);

    std::string m_LevelKey;
    LevelStats m_Stats;
    bool m_Linked;
    DTLinkLayer* m_DTLinkLayer;

    void MoveMe(CCObject*);
};