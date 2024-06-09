#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

class DTManageLevelsLayer : public Popup<DTLayer* const&>, public TextInputDelegate, public LevelManagerDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;

    public:
        static DTManageLevelsLayer* create(DTLayer* const& layer);

        DTLayer* m_DTLayer;

        CCNode* alighmentNode;

        std::vector<std::pair<std::string, LevelStats>> m_AllLevels;

        float CellsWidth = 300;

        InputNode* seartchInput;
        std::string m_filterText;
        void textChanged(CCTextInputNode* input);

        void refreshLists(bool SavePos);

        void SpacialEditList(GJListLayer* list, CCPoint titlePos, float titleSize);

        GJListLayer* m_LevelsList = nullptr;

        bool dInfo;

        LoadingCircle* downloadCircle;

        //info
        void onOverallInfo(CCObject*);
};