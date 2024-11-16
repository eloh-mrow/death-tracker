#pragma once

#include <Geode/Geode.hpp>
#include "../utils/Save.hpp"
#include "../layers/DTLayer.hpp"
#include <Geode/ui/TextArea.hpp>

class LabelLayoutWindow : public cocos2d::CCNode {
protected:
    bool init(const LabelLayout& MyLayout, DTLayer* const& DTLayer);
public:
    static LabelLayoutWindow* create(const LabelLayout& MyLayout, DTLayer* const& DTLayer);

    void myUpdate(float delta);

    bool m_FollowMouse;
    CCScale9Sprite* s;

    LabelLayout m_MyLayout;
    LabelLayout m_MyLayoutSave;
    SimpleTextArea* m_Label;
    bool m_Lock;

    DTLayer* m_DTLayer;

    CCPoint mousePosToNode(CCNode* const& node);
    bool isMouseTouching(CCNode* const& node);
    void setPositionBasedOnLayout(const LabelLayout& layout, int d = 0);
    //first: line, second: position
    std::pair<int, int> getLineByPos(CCPoint pos);
    void updateLine(const int& line);
    float m_DoubleClickTimer;
    bool isNextToAnother();

    bool MoveEnabled = true;
    void setMoveEnabled(const bool& b);
    bool oneTimeClickDetect;

    void setOpacity(GLubyte opacity);
    void runAction(CCAction* action);
};
