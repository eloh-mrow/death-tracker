#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

class DTLayer : public Popup<GJGameLevel* const&>, public TextInputDelegate, public FLAlertLayerProtocol {
    protected:
        bool setup(GJGameLevel* const& Level) override;

        void update(float delta) override;
    public:
        static DTLayer* create(GJGameLevel* const& Level);

        //data
        GJGameLevel* m_Level;
        LevelStats m_MyLevelStats;
        bool m_noSavedData;

        LevelStats m_SharedLevelStats;
        
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
        std::vector<std::tuple<std::string, int, float>> CreateRunsString(Runs runs);
        void refreshStrings();

        std::vector<std::tuple<std::string, int, float>> m_DeathsInfo;
        std::vector<std::tuple<std::string, int, float>> selectedSessionInfo;
        std::vector<std::tuple<std::string, int, float>> m_RunInfo;
        std::vector<std::tuple<std::string, int, float>> m_SelectedSessionRunInfo;

        std::string deathsString;
        std::string selectedSessionString;

        std::string RunString;
        std::string selectedSessionRunString;

        //run managment
        GJListLayer* m_RunsList = nullptr;
        InputNode* m_AddRunAllowedInput;
        CCMenu* m_RunStuffMenu;
        void addRunAllowed(CCObject*);
        void updateRunsAllowed();
        void refreshRunAllowedListView();
        void deleteUnused(CCObject*);
        FLAlertLayer* m_RunDeleteAlert;
        CCMenuItemToggler* allRunsToggle;
        void OnToggleAllRuns(CCObject*);

        //session selection
        InputNode* m_SessionSelectionInput = nullptr;
        CCMenu* m_SessionSelectMenu;
        int m_SessionsAmount;
        int m_SessionSelected;
        bool m_SessionSelectionInputSelected;
        void updateSessionString(int session);
        void SwitchSessionRight(CCObject*);
        void SwitchSessionLeft(CCObject*);

        //linking
        void OnLinkButtonClicked(CCObject*);
        void UpdateSharedStats();

        //manage
        void OnManage(CCObject*);

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
        CCTouch* ClickPos = nullptr;

        void textChanged(CCTextInputNode* input) override;
        void textInputOpened(CCTextInputNode* input) override;
        void textInputClosed(CCTextInputNode* input) override;

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected);

        void onClose(cocos2d::CCObject*) override;

        //graph
        void openGraphMenu(CCObject*);

        //settings
        void onSettings(CCObject*);

        //modify runs
        InputNode* addFZRunInput;
        InputNode* addRunStartInput;
        InputNode* addRunEndInput;
        InputNode* runsAmountInput;
        CCMenu* modifyRunsMenu;

        void onAddedFZRun(CCObject*);
        void onRemovedFZRun(CCObject*);

        void onAddedRun(CCObject*);
        void onRemovedRun(CCObject*);
};