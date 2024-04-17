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

        CCNode* CreateGraph(std::vector<std::tuple<std::string, int, float>> deathsString, float bestRun, ccColor3B color, CCPoint Scaling, ccColor4B graphBoxOutlineColor, ccColor4B graphBoxFillColor, float graphBoxOutlineThickness, ccColor4B labelLineColor, ccColor4B labelColor, int labelEvery, ccColor4B gridColor, int gridLineEvery);
        float GetBestRun(NewBests bests);

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
        void onViewModeButton(CCObject*);
        bool ViewModeNormal = true;

        void refreshGraph();
        CCNode* m_graph = nullptr;
        CCLabelBMFont* noGraphLabel;
};