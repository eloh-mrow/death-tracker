#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

class DTLayer : public Popup<GJGameLevel* const&>, public TextInputDelegate {
    protected:
        bool setup(GJGameLevel* const& Level) override;

        void update(float delta) override;
    public:
        static DTLayer* create(GJGameLevel* const& Level);

        //data
        GJGameLevel* m_Level;
        LevelStats m_MyLevelStats;
        bool m_noSavedData;
        
        //main page
        CCScale9Sprite* m_TextBG;
        ScrollLayer* m_ScrollLayer;
        Scrollbar* m_ScrollBar;

        //text
        void RefreshText(bool moveToTop = false);
        CCNode* m_TextCont = nullptr;
        std::string modifyString(std::string ToModify);
        bool isKeyInIndex(std::string s, int Index, std::string key);

        std::vector<std::tuple<std::string, int, float>> CreateDeathsString(Deaths deaths, NewBests newBests, std::string NewBestsColorString);
        std::vector<std::tuple<std::string, int>> CreateRunsString(Runs runs);
        void refreshStrings();

        std::vector<std::tuple<std::string, int, float>> m_DeathsInfo;
        std::vector<std::tuple<std::string, int, float>> selectedSessionInfo;

        std::string deathsString;
        std::string selectedSessionString;

        //session selection
        InputNode* m_SessionSelectionInput;
        CCMenu* m_SessionSelectMenu;
        int m_SessionsAmount;
        int m_SessionSelected;
        bool m_SessionSelectionInputSelected;
        void updateSessionString(int session);
        void SwitchSessionRight(CCObject*);
        void SwitchSessionLeft(CCObject*);

        //edit layout mode
        CCMenuItemSpriteExtra* m_EditLayoutBtn;
        void onEditLayout(CCObject*);
        CCMenu* m_EditLayoutMenu;
        CCSprite* m_BlackSquare;
        void onEditLayoutCancle(CCObject*);
        void onEditLayoutApply(CCObject*);
        void EditLayoutEnabled(bool b);
        std::vector<CCNode*> m_LayoutLines;
        CCNode* m_LayoutStuffCont;
        void createLayoutBlocks();
        bool m_IsMovingAWindow;
        void changeScrollSizeByBoxes(bool moveToTop = false);
        void addBox(CCObject*);

        //general
        bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override;
        bool m_IsClicking;

        void textChanged(CCTextInputNode* input) override;
        void textInputOpened(CCTextInputNode* input) override;
        void textInputClosed(CCTextInputNode* input) override;
};