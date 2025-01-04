#pragma once

#include <Geode/Geode.hpp>
#include "../managers/StatsManager.hpp"
#include "../nodes/GraphPoint.hpp"

using namespace cocos2d;
using namespace geode;

class DTGraphNode : public CCNode {
    protected:
        bool init(const CCSize& scaling);
        
    public:
        static DTGraphNode* create(const CCSize& scaling);

        enum GraphType{
            PassRate,
            ReachRate
        };

        void setBGFillColor(const ccColor4B& color);
        void setOutlineThickness(float thickness);
        void setOutlineColor(const ccColor4B& color);

        void refreshBackground(const ccColor4B& newFillColor, float newOutlineThickness, const ccColor4B& newOutlineColor);

        void addGraphForDeaths(const std::string& graphName, const std::vector<DeathInfo>& deaths, GraphType type, float thickness, const ccColor4B& color, bool clearOther = false);
        void setGraphColorByName(const std::string& graphName, const ccColor4B& newColor);
        void eraseGraphByName(const std::string& graphName);

        void refreshGrid();

        void updateLabels();

        void setSmallLinesColor(const ccColor4B& newColor);
        void setBoldLinesColor(const ccColor4B& newColor);
        void setLabelsColor(const ccColor4B& newColor);

    private:

        struct GraphLine {
            CCDrawNode* lineNode = nullptr;
            std::vector<CCPoint> points{};
            float thickness = 1;
            CCMenu* GraphPointsContainer = nullptr;
        };

        CCSize scaling;

        ccColor4B fillColor = {0, 0, 0, 120};
        float outlineThickness = 0.2f;
        ccColor4B outlineColor = { 124, 124, 124, 255};

        int labelEvery = 5;

        CCDrawNode* boldGridNode = nullptr;
        ccColor4B boldGridColor = { 29, 29, 29, 255 };

        ccColor4B boldLineColor = {115, 115, 115, 255};
        ccColor4B smallLineColor = {115, 115, 115, 100};
        ccColor4B labelColor = { 202, 202, 202, 255};

        CCDrawNode* fillNode = nullptr;
        CCDrawNode* outlineNode = nullptr;
        CCClippingNode* mask = nullptr;

        CCNode* graphsContainer = nullptr;
        CCNode* LabelsContainer = nullptr;

        std::unordered_map<std::string, GraphLine> allGraphs{};
        std::set<CCSprite*> smallLines{};
        std::set<CCSprite*> boldLines{};
        std::set<CCLabelBMFont*> labels{};
};