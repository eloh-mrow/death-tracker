#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class ChooseRunCell : public CCNode {
protected:
    bool init(const int& Precent, const std::function<void(const int&)>& callback);
public:
    static ChooseRunCell* create(const int& Precent, const std::function<void(const int&)>& callback = NULL);

    int m_Precent;
    std::function<void(const int&)> m_Callback;

    void ChooseMe(CCObject*);
};