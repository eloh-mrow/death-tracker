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

        // creates a graph with a given deaths string
        // @param deathsString the deaths info to make the graph out of
        // @param bestRun the best run of the deaths info
        // @param color the color the graph will be
        // @param scaling determines the scale of the graph
        // @param graphBoxOutlineColor the color of the graph outline
        // @param graphBoxFillColor the color of the graph background
        // @param graphBoxOutlineThickness the thickness of the graphs box outline
        // @param labelLineColor the color of the small lines next to the numebrs
        // @param labelColor color of the numbers displayed on the sides
        // @param labelEvery how often a number label appears
        // @param gridColor the color of the grid lines
        // @param gridLineEvery how often a grid line appears
        // @param type the type of graph to create
        /*
        CCNode* CreateGraph(
            const std::vector<DeathInfo>& deathsString, const int& bestRun, const ccColor3B& color,
            const CCPoint& scaling, const ccColor4B& graphBoxOutlineColor, const ccColor4B& graphBoxFillColor, const float& graphBoxOutlineThickness,
            const ccColor4B& labelLineColor, const ccColor4B& labelColor, const int& labelEvery, const ccColor4B& gridColor, const int& gridLineEvery, const GraphType& type
        );*/
        //get the best run in a runs vector
        static int GetBestRun(const std::vector<DeathInfo>& selectedPercentRunInfo);

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