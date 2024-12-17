#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"
#include "../../managers/StatsManager.hpp"
#include "../../utils/Save.hpp"

using namespace geode::prelude;

class DTLinkLayer : public Popup<DTLayer* const&>, public TextInputDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;

        void update(float delta) override;
    public:
        static DTLinkLayer* create(DTLayer* const& layer);

    private:
    
        //edits a lists title position and tize
        void SpacialEditList(GJListLayer* const& list, const CCPoint& titlePos, const float& titleSize);
        //refreshes the linked and levels lists
        void refreshLists();
        //change a levels liked state
        void ChangeLevelLinked(const std::string levelKey, LevelStats stats, const bool& erase);

        //update the search results
        void textChanged(CCTextInputNode* input) override;

        //handle closing and refresh stuff
        void onClose(CCObject*) override;
        //info about the link layer and its uses 
        void onOverallInfo(CCObject*);

        //move a page left on the levels list
        void levelsListMoveLeft(CCObject*);
        //move a page right on the levels list
        void levelsListMoveRight(CCObject*);

        CCNode* alignmentNode;
        DTLayer* m_DTLayer;
        std::vector<std::pair<std::string, LevelStats>> m_AllLevels;

        float CellsWidth = 230;

        bool scrollSwitchLock;

        GJListLayer* m_LevelsList = nullptr;
        int levelPage = 1;
        GJListLayer* m_LinkedLevelsList = nullptr;

        TextInput* seartchInput;
        std::string m_filterText;

        CCMenuItemSpriteExtra* levelsMoveRight;
        CCMenuItemSpriteExtra* levelsMoveLeft;
};