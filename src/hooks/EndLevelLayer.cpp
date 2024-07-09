#if defined(GEODE_IS_MACOS)

#include <Geode/modify/EndLevelLayer.hpp>
#include "../hooks/DTPlayLayer.hpp"
#include "../hooks/DTCCMenuItemSpriteExtra.hpp"

using namespace geode::prelude;

class $modify(DTEndLevelLayer, EndLevelLayer) {

    struct Fields {
        DTCCMenuItemSpriteExtra* resetButton = nullptr;
        bool lockThing = false;
    };

    void customSetup(){
        EndLevelLayer::customSetup();

        m_fields->resetButton = static_cast<DTCCMenuItemSpriteExtra*>(m_mainLayer->getChildByID("button-menu")->getChildByID("retry-button"));

        schedule(schedule_selector(DTEndLevelLayer::myUpdate));
    }

    void myUpdate(float delta){
        if (m_fields->resetButton->m_fields->didActivate && !m_fields->lockThing){
            m_fields->lockThing = true;
            
            static_cast<DTPlayLayer*>(GameManager::get()->getPlayLayer())->m_fields->currentRun.end = 0;
        }
    }
};

#endif