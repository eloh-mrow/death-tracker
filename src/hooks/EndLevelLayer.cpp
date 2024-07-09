#if defined(GEODE_IS_MACOS)
#include <Geode/modify/EndLevelLayer.hpp>
#include "../hooks/DTPlayLayer.hpp"

using namespace geode::prelude;

class $modify(DTEndLevelLayer, EndLevelLayer) {
    void onReplay(cocos2d::CCObject* sender){
        EndLevelLayer::onReplay(sender);

        static_cast<DTPlayLayer*>(GameManager::get()->getPlayLayer())->m_fields->currentRun.end = 0;

        //log::info("EndLevelLayer::onReplay()");
    }
};
#endif