#pragma once

#include <Geode/Geode.hpp>
#include "../managers/StatsManager.hpp"
#include "../layers/DTLinkLayer.hpp"

using namespace cocos2d;
using namespace geode;

class LinkLevelCell : public CCNode {
    protected:
        bool init(const float& cellW, const std::string& levelKey, const LevelStats& stats, const bool& linked, const std::function<void(const std::string, LevelStats, const bool&)>& callback);
    public:
        static LinkLevelCell* create(const float& cellW, const std::string& levelKey, const LevelStats& stats, const bool& linked, const std::function<void(const std::string, LevelStats, const bool&)>& callback = NULL);

    private:
        //runs the given callback to this LinkLevelCell
        void MoveMe(CCObject*);

        std::string m_LevelKey;
        LevelStats m_Stats;
        bool m_Linked;
        std::function<void(const std::string, LevelStats, const bool&)> m_Callback;
};