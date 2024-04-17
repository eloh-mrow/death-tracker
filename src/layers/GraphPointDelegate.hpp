#pragma once

#include <Geode/Geode.hpp>

class GraphPointDelegate {
    public:
        virtual void OnPointSelected(cocos2d::CCNode* point){};

        virtual void OnPointDeselected(cocos2d::CCNode* point){};
};