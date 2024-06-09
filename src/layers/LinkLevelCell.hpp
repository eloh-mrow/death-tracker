#pragma once

#include <Geode/Geode.hpp>
#include "../managers/StatsManager.hpp"
#include "../layers/DTLinkLayer.hpp"
#include "../layers/DTManageLevelsLayer.hpp"

using namespace cocos2d;
using namespace geode;

class LinkLevelCell : public CCNode, public FLAlertLayerProtocol, public LevelManagerDelegate {
protected:
    bool init(CCNode* l, std::string levelKey, LevelStats stats, bool linked);
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

    void loadLevelsFinished(cocos2d::CCArray* p0, char const* p1);
    void loadLevelsFinished(cocos2d::CCArray* p0, char const* p1, int p2);

    void loadLevelsFailed(char const* p0);
    void loadLevelsFailed(char const* p0, int p1);

    bool downloadingInfo;

    void FLAlert_Clicked(FLAlertLayer* alert, bool selected);
    FLAlertLayer* DeleteWarningAlert;
};