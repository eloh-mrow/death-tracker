#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class LabelFontCell : public CCNode {
protected:
    bool init(const int& FontID, const std::string& FontFNT, const std::string& FontName, const std::function<void(const int&)>& callback);
public:
    static LabelFontCell* create(const int& FontID, const std::string& FontFNT, const std::string& FontName, const std::function<void(const int&)>& callback = NULL);

    int m_FontID;
    std::function<void(const int&)> m_Callback;

    void Use(CCObject*);
};