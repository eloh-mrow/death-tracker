#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"
#include "../GraphPoint.hpp"
#include "../DTGraphNode.hpp"

using namespace geode::prelude;

class DTGraphLayer : public Popup<DTLayer* const&>, public TextInputDelegate, public GraphPointDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;
    public:
        static DTGraphLayer* create(DTLayer* const& layer);

    private:
        //graph stuff

        //change the displayed point
        void OnPointSelected(cocos2d::CCNode* point) override;
        //have no point displayed if the deselected point was the was selected prior
        void OnPointDeselected(cocos2d::CCNode* point) override;

        //update the graph according to the other settings
        void refreshGraph();

        CCLabelBMFont* noGraphLabel;
        GraphPoint* pointToDisplay;
        CCLabelBMFont* npsLabel;
        SimpleTextArea* PointInfoLabel;

        
        //selection stuff

        //increas the session selected by 1
        void switchedSessionRight(CCObject*);
        //decrease the session selected by 1
        void switchedSessionLeft(CCObject*);

        TextInput* m_SessionSelectionInput;

        void textChanged(CCTextInputNode* input) override;
        void textInputClosed(CCTextInputNode* input) override;
        void textInputOpened(CCTextInputNode* input) override;

        //toggle between 'normal' and 'session' view
        void onViewModeButton(CCObject*);
        //toggle between 'normal' and 'run' view
        void onRunViewModeButton(CCObject*);
        //toggle between different graph types
        void onTypeViewModeButton(CCObject*);

        bool ViewModeNormal = true;
        bool RunViewModeFromZero = true;
        ButtonSprite* viewModeButtonS;
        ButtonSprite* runViewModeButtonS;
        ButtonSprite* typeViewModeButtonS;

        //change the current run selected for the graph
        void RunChosen(const int& run);

        TextInput* m_RunSelectInput;
        int m_SelectedRunPercent;
        GJListLayer* m_RunsList;
        
        //info

        //overall info about the graph
        void onOverallInfo(CCObject*);

        //handle the layer closing and refresh the text
        void onClose(cocos2d::CCObject*) override;

        DTLayer* m_DTLayer;
        CCNode* alignmentNode;
        //GraphType currentType = GraphType::PassRate;
        DTGraphNode* graph;
};