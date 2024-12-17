#include <Geode/modify/LevelInfoLayer.hpp>
#include "../nodes/layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Settings.hpp"

using namespace geode::prelude;

class $modify(DTLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* btn = nullptr;
    };
    
    bool init(GJGameLevel* p0, bool p1);

    void checkIfPlayVisible(float delta);

    void openDTLayer(CCObject*);
};