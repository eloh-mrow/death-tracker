#include <Geode/modify/EditLevelLayer.hpp>
#include "../nodes/layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Settings.hpp"

using namespace geode::prelude;

class $modify(DTEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level);

    void openDTLayer(CCObject*);
};
