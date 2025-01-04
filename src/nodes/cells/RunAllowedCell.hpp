#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class RunAllowedCell : public CCMenuItemSpriteExtra {
    protected:
        bool init(const int& Percent, const float& scale, const std::function<void(const int&, cocos2d::CCNode*)>& callback);
    public:
        static RunAllowedCell* create(const int& Percent, const float& scale, const std::function<void(const int&, cocos2d::CCNode*)>& callback = NULL);

        ButtonSprite* buttonSprite;
        
    private:
        //runs the given callback to this RunAllowedCell
        void DeleteMe(CCObject*);

        int m_Percent;
        std::function<void(int, cocos2d::CCNode*)> m_Callback;
};