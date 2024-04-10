#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class LabelFontCell : public CCNode {
protected:
    bool init(int FontID, std::string FontFNT, std::string FontName, CCNode* SettingsLayer);
public:
    static LabelFontCell* create(int FontID, std::string FontFNT, std::string FontName, CCNode* SettingsLayer);

    int m_FontID;
    std::string m_FontFNT;
    std::string m_FontName;
    CCNode* m_SettingsLayer;

    void Use(CCObject*);
};