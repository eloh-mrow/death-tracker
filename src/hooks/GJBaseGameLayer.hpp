#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(DTGJBaseGameLayer, GJBaseGameLayer) {
    void checkpointActivated(CheckpointGameObject* checkpt);
};