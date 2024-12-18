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

        //set the delegate for the point
        void setDelegate(GraphPointDelegate* Delegate);

        std::string m_Run;

        float m_Passrate;

    private:
        //check for inputs
        void update(float delta) override;

        CCSprite* c;

        GraphPointDelegate* m_Delegate = nullptr;
        bool m_oneTimeCall;
};