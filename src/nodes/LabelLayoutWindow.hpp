#pragma once

#include <Geode/Geode.hpp>
#include "../utils/Save.hpp"
#include "../nodes/layers/DTLayer.hpp"

class LabelLayoutWindow : public cocos2d::CCNode {
    protected:
        bool init(const LabelLayout& MyLayout, DTLayer* const& DTLayer);
    public:
        static LabelLayoutWindow* create(const LabelLayout& MyLayout, DTLayer* const& DTLayer);

        //sets this windows offset and scale based on this layout info
        void setPositionBasedOnLayout(const LabelLayout& layout, int d = 0);

        //if this window is next to another window horizontally, return true 
        bool isNextToAnother();

        //enable/disable this windows movement
        void setMoveEnabled(const bool& b);

        DTLayer* m_DTLayer;

        LabelLayout m_MyLayout;
        CCScale9Sprite* s;
        SimpleTextArea* m_Label;

    private:

        //used to detect inputs, and double clicks
        void myUpdate(float delta);
        float m_DoubleClickTimer;

        //converts a mouse input to node space
        CCPoint mousePosToNode(CCNode* const& node);
        //if the curser is clicking this window, returns true
        bool isMouseTouching(CCNode* const& node);
        //gets the labels layout position based on its local node position
        //first: line, second: position
        std::pair<int, int> getLineByPos(CCPoint pos);
        //updates position of recieves the same line number
        void updateLine(const int& line);

        //set this windows opacity
        void setOpacity(GLubyte opacity);
        //run a CCAction on this window
        void runAction(CCAction* action);

        bool MoveEnabled = true;
        bool oneTimeClickDetect;

        bool m_FollowMouse;

        LabelLayout m_MyLayoutSave;
        bool m_Lock;
};
