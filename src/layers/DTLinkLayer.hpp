#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

class DTLinkLayer : public Popup<DTLayer* const&>, public TextInputDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;

        void update(float delta) override;
    public:
        static DTLinkLayer* create(DTLayer* const& layer);

        CCNode* alignmentNode;

        DTLayer* m_DTLayer;

        void SpacialEditList(GJListLayer* const& list, const CCPoint& titlePos, const float& titleSize);

        void refreshLists();

        void ChangeLevelLinked(const std::string levelKey, LevelStats stats, const bool& erase);

        float CellsWidth = 230;

        bool scrollSwitchLock;

        GJListLayer* m_LevelsList = nullptr;
        int levelPage = 1;
        GJListLayer* m_LinkedLevelsList = nullptr;

        TextInput* seartchInput;
        std::string m_filterText;
        void textChanged(CCTextInputNode* input) override;

        std::vector<std::pair<std::string, LevelStats>> m_AllLevels;

        void onClose(CCObject*) override;
        //info
        void onOverallInfo(CCObject*);

        CCMenuItemSpriteExtra* levelsMoveRight;
        CCMenuItemSpriteExtra* levelsMoveLeft;
        void levelsListMoveLeft(CCObject*);
        void levelsListMoveRight(CCObject*);
};