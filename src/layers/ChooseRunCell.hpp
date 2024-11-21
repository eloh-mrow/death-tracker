#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class ChooseRunCell : public CCNode {
    protected:
        bool init(const int& Percent, const std::function<void(const int&)>& callback);

    public:
        static ChooseRunCell* create(const int& Percent, const std::function<void(const int&)>& callback = NULL);

    private:
        //runs the callback provided when the use button is clicked
        void ChooseMe(CCObject*);
        

        int m_Percent;
        std::function<void(const int&)> m_Callback;
};