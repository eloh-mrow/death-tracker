#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class GraphPointDelegate {
    public:
        virtual void OnPointSelected(cocos2d::CCNode* point){};

        virtual void OnPointDeselected(cocos2d::CCNode* point){};
};

class GraphPoint : public CCMenuItem {
    protected:
        bool init(const std::string& run, const float& passrate, const ccColor3B& color);
    public:
        static GraphPoint* create(const std::string& run, const float& passrate, const ccColor3B& color);

        void update(float delta) override;

        CCSprite* c;

        std::string m_Run;

        float m_Passrate;

        GraphPointDelegate* m_Delegate;
        bool m_oneTimeCall;

        void setDelegate(GraphPointDelegate* Delegate);
};