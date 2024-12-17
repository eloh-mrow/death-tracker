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

        void setBGFillColor(ccColor4B color);
        void setOutlineThickness(float thickness);
        void setOutlineColor(ccColor4B color);

        void refreshBackground(ccColor4B newFillColor, float newOutlineThickness, ccColor4B newOutlineColor);

        void addGraphForDeaths(const std::string& graphName, const std::vector<DeathInfo>& deaths, GraphType type, float thickness, const ccColor4B& color, bool clearOther = false);
        void setGraphColorByName(const std::string& graphName, const ccColor4B& newColor);
        void eraseGraphByName(const std::string& graphName);

        void refreshGizmos();

    private:

        struct GraphLine {
            CCDrawNode* lineNode = nullptr;
            std::vector<CCPoint> points{};
            float thickness = 1;
            CCMenu* GraphPointsContainer = nullptr;
        };

        CCSize scaling;

        ccColor4B fillColor = { 124, 124, 124, 255};
        float outlineThickness = 0.2f;
        ccColor4B outlineColor = {0, 0, 0, 120};

        CCDrawNode* BGNode = nullptr;
        CCClippingNode* mask = nullptr;

        std::unordered_map<std::string, GraphLine> allGraphs{};
};