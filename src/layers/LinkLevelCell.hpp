#pragma once

#include <Geode/Geode.hpp>
#include "../managers/StatsManager.hpp"
#include "../layers/DTLinkLayer.hpp"
#include "../layers/DTManageLevelsLayer.hpp"

using namespace cocos2d;
using namespace geode;

class LinkLevelCell : public CCNode {
protected:
    bool init(CCNode* l, std::string levelKey, LevelStats stats, bool linked);

    void update(float delta);
public:
    static LinkLevelCell* create(DTLinkLayer* DTL, std::string levelKey, LevelStats stats, bool linked);
    static LinkLevelCell* create(DTManageLevelsLayer* DTMLL, std::string levelKey, LevelStats stats, bool linked);

    std::string m_LevelKey;
    LevelStats m_Stats;
    bool m_Linked;
    DTLinkLayer* m_DTLinkLayer;
    DTManageLevelsLayer* m_DTManageLevelsLayer;

    void MoveMe(CCObject*);
    void DeleteMe(CCObject*);
    void ViewMe(CCObject*);

    bool downloadingInfo;
    LevelListLayer* m_LoadLevelsBypass = nullptr;
    LoadingCircle* m_LoadLevelsCircle = nullptr;
    int loadingProgress;
};