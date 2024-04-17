#pragma once

#include <Geode/Geode.hpp>
#include "../layers/GraphPointDelegate.hpp"

using namespace cocos2d;
using namespace geode;

class GraphPoint : public CCMenuItem {
    protected:
        bool init(std::string run, float passrate, ccColor3B color);
    public:
        static GraphPoint* create(std::string run, float passrate, ccColor3B color);

        void update(float delta);

        CCSprite* c;

        std::string m_Run;

        float m_Passrate;

        GraphPointDelegate* m_Delegate;
        bool m_oneTimeCall;

        void setDelegate(GraphPointDelegate* Delegate);
};