#pragma once

#include <Geode/Geode.hpp>
#include "../utils/Save.hpp"
#include "../layers/DTLayer.hpp"
#include <Geode/ui/TextArea.hpp>

class LabelLayoutWindow : public cocos2d::CCNode {
protected:
    bool init(LabelLayout MyLayout, DTLayer* DTLayer);
public:
    static LabelLayoutWindow* create(LabelLayout MyLayout, DTLayer* DTLayer);

    void myUpdate(float delta);

    bool m_FollowMouse;
    CCScale9Sprite* s;

    LabelLayout m_MyLayout;
    LabelLayout m_MyLayoutSave;
    SimpleTextArea* m_Label;
    bool m_Lock;

    DTLayer* m_DTLayer;

    CCPoint mousePosToNode(CCNode* node);
    bool isMouseTouching(CCNode* node);
    void setPositionBasedOnLayout(LabelLayout layout, int d = 0);
    //first: line, second: position
    std::pair<int, int> getLineByPos(CCPoint pos);
    void updateLine(int line);
    float m_DoubleClickTimer;
};
