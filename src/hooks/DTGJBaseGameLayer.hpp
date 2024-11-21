#if !defined(GEODE_IS_MACOS)

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(DTGJBaseGameLayer, GJBaseGameLayer) {
    void checkpointActivated(CheckpointGameObject* checkpt);
};

#endif