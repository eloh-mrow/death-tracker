#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"
#include "../layers/GraphPoint.hpp"
#include "Geode/ui/TextArea.hpp"
#include "../layers/GraphPointDelegate.hpp"

using namespace geode::prelude;

class DTGraphLayer : public Popup<DTLayer* const&>, public TextInputDelegate, public GraphPointDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;

        void update(float delta);
    public:
        static DTGraphLayer* create(DTLayer* const& layer);

        DTLayer* m_DTLayer;

        CCNode* alighmentNode;

        CCNode* CreateGraph(std::vector<std::tuple<std::string, int, float>> deathsString, int bestRun, ccColor3B color, CCPoint Scaling, ccColor4B graphBoxOutlineColor, ccColor4B graphBoxFillColor, float graphBoxOutlineThickness, ccColor4B labelLineColor, ccColor4B labelColor, int labelEvery, ccColor4B gridColor, int gridLineEvery);
        CCNode* CreateRunGraph(std::vector<std::tuple<std::string, int, float>> deathsString, int bestRun, ccColor3B color, CCPoint Scaling, ccColor4B graphBoxOutlineColor, ccColor4B graphBoxFillColor, float graphBoxOutlineThickness, ccColor4B labelLineColor, ccColor4B labelColor, int labelEvery, ccColor4B gridColor, int gridLineEvery);
        int GetBestRun(NewBests bests);
        int GetBestRunDeathS(std::vector<std::tuple<std::string, int, float>> selectedPrecentDeathsInfo);
        int GetBestRun(std::vector<std::tuple<std::string, int, float>> selectedPrecentRunInfo);

        std::vector<GraphPoint*> pointToDisplay;
        CCLabelBMFont* npsLabel;
        SimpleTextArea* PointInfoLabel;

        void OnPointSelected(cocos2d::CCNode* point);

        void OnPointDeselected(cocos2d::CCNode* point);

        void switchedSessionRight(CCObject*);
        void switchedSessionLeft(CCObject*);
        InputNode* m_SessionSelectionInput;
        void textChanged(CCTextInputNode* input);
        void textInputClosed(CCTextInputNode* input);
        void textInputOpened(CCTextInputNode* input);

        void onClose(cocos2d::CCObject*);

        ButtonSprite* viewModeButtonS;
        ButtonSprite* runViewModeButtonS;
        void onViewModeButton(CCObject*);
        void onRunViewModeButton(CCObject*);
        bool ViewModeNormal = true;
        bool RunViewModeFromZero = true;

        void refreshGraph();
        CCNode* m_graph = nullptr;
        CCLabelBMFont* noGraphLabel;

        InputNode* m_RunSelectInput;
        int m_SelectedRunPrecent;
        GJListLayer* m_RunsList;
        void RunChosen(int run);
};