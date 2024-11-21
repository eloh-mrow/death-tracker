#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

using DeathStringTask = Task<Result<std::vector<DeathInfo>>>;
using ResultTask = Task<Result<>>;

class DTLayer : public Popup<GJGameLevel* const&>, public TextInputDelegate, public FLAlertLayerProtocol, public ColorPickPopupDelegate {
    protected:
        bool setup(GJGameLevel* const& Level) override;

    public:
        static DTLayer* create(GJGameLevel* const& Level);

        //will look for any special keys in the given string and output a formatted string accordingly
        std::string modifyString(std::string ToModify);

        //updates the shared stats, the shared stats will contain all deaths and runs from all linked levels including the current one
        void UpdateSharedStats();

        //activates when clicking a window
        //
        //currently only used for copying the windows text
        void clickedWindow(CCNode* window);

        //changes the size of the main scroll layer depending on the layout boxes added
        void changeScrollSizeByBoxes(const bool& moveToTop = false);

        //saves and refreshes the main page text
        void updateRunsAllowed();

        //refreshes all the text, this includes all run and death infos, strings, session strings and the main pages labels
        //this whole operation happens on a task in the background and activates the refresh circle until finished
        void refreshAll(bool moveToTop = false);
        //refreshes session related stuff, this includes session run and death infos, session strings and the main pages labels
        //this whole operation happens on a task in the background and activates the refresh circle until finished
        void refreshSession(bool moveToTop = false);

        void onClose(cocos2d::CCObject*) override;

        GJGameLevel* m_Level;
        ScrollLayer* m_ScrollLayer;

        LevelStats m_MyLevelStats;
        LevelStats m_SharedLevelStats;

        std::vector<DeathInfo> m_DeathsInfo;
        std::vector<DeathInfo> selectedSessionInfo;
        std::vector<DeathInfo> m_RunInfo;
        std::vector<DeathInfo> m_SelectedSessionRunInfo;
        std::string deathsString;
        std::string selectedSessionString;

        bool m_IsMovingAWindow;
        std::vector<CCNode*> m_LayoutLines;
        ColorChannelSprite* colorSpritenb;
        ColorChannelSprite* colorSpritesb;

        std::string RunString;
        std::string selectedSessionRunString;

        bool m_IsClicking;
        CCTouch* ClickPos = nullptr;

        TextInput* m_SessionSelectionInput = nullptr;
        int m_SessionsAmount;
        int m_SessionSelected;
        bool m_SessionSelectionInputSelected;

        CCNode* LevelSpecificSettingsLayer = nullptr;

    private:
        void update(float delta) override;

        void updateColor(cocos2d::ccColor4B const& color) override;

        //text

        //update the displayed text
        void RefreshText(bool moveToTop = false);

        CCNode* m_TextCont = nullptr;

        //gets and processes the saved deaths for the current level, outputs result into 'm_DeathsInfo'
        DeathStringTask CreateDeathsInfo(const Deaths& deaths, const NewBests& newBests);
        //gets and processes the saved runs for the current level, outputs result into 'm_RunInfo'
        DeathStringTask CreateRunsInfo(const Runs runs);
        //uses the deaths and runs info to create a long string to be displayed in the main page
        ResultTask refreshStrings();
        //uses the deaths and runs info to create a long string to be displayed in the main page, this creates the string specifically for the current session selected
        ResultTask updateSessionString(const int& session);

        //session selection
        
        //switches the session selected one to the right
        void SwitchSessionRight(CCObject*);
        //switches the session selected one to the left
        void SwitchSessionLeft(CCObject*);

        CCMenu* m_SessionSelectMenu;

        //linking

        //opens the link menu
        void OnLinkButtonClicked(CCObject*);
        CCMenuItemSpriteExtra* LinkLevelsButton;

        //edit layout mode
        
        //toggles the edit layout menu on
        void onEditLayout(CCObject*);
        
        //activates when clicking the apply button on the edit layout menu, will save all changes to the main layout
        void onEditLayoutApply(CCObject*);

        //toggles edit layout menu on or off
        void EditLayoutEnabled(const bool& b);
        
        //creates the layout label blocks representing the text, and deletes the old one
        void createLayoutBlocks();
        
        //adds a new layout label
        void addBox(CCObject*);
        
        //resets the layout to the default layout
        void onResetLayout(CCObject*);
        //opens the color picker layer for editing the new bests color
        void editnbcColor(CCObject*);
        //opens the color picker layer for editing the session bests color
        void editsbcColor(CCObject*);

        CCMenu* m_EditLayoutMenu;
        CCSprite* m_BlackSquare;

        CCNode* m_LayoutStuffCont = nullptr;

        CCMenuItemSpriteExtra* editLayoutApplyBtn;
        CCMenuItemSpriteExtra* addWindowButton;
        CCMenuItemSpriteExtra* layoutInfoButton;

        FLAlertLayer* ResetLayoutAlert;
        CCMenuItemSpriteExtra* resetLayoutButton;

        geode::ColorPickPopup* colorSelectnb;

        geode::ColorPickPopup* colorSelectsb;

        CCMenuItemSpriteExtra* m_EditLayoutBtn;

        bool openednbLast;

        //general

        bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
        void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override;

        void textChanged(CCTextInputNode* input) override;
        void textInputOpened(CCTextInputNode* input) override;
        void textInputClosed(CCTextInputNode* input) override;

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected) override;

        //graph

        //opens the graph menu
        void openGraphMenu(CCObject*);

        //settings

        //opens the geode setting menu for the mod
        void onSettings(CCObject*);
        CCMenuItemSpriteExtra* settingsButton;

        //info about the labels and edit layout menu
        void onLayoutInfo(CCObject*);
        //info about copying label text
        void onCopyInfo(CCObject*);

        //copy

        //opens the edit layout menu and modifies it to let you click a label box to copy its text
        void copyText(CCObject*);
        bool isInCopyMenu;
        CCMenuItemSpriteExtra* copyInfoButton;
        CCMenuItemSpriteExtra* copyTextButton;

        //specific settings

        //opens the level specific settings layer
        void onSpecificSettings(CCObject*);
        //handle the main and specific settings layer after their enter/exit transition finishes
        void onMoveTransitionEnded(CCObject* LSSL);

        bool runningMoveTransition = false;
        bool isExitingSSLayer = false;
        CCSprite* levelSettingsBSArrow;

        //disables the refresh circle once a refresh process is finished
        void onRefreshFinished(ResultTask::Event* event);

        EventListener<ResultTask> refreshListener;
        LoadingCircle* refreshLoadingCircle = nullptr;

        //data

        bool m_noSavedData;
        
        //main page
        CCScale9Sprite* m_TextBG;
        Scrollbar* m_ScrollBar;
        CCNode* alignmentNode;
};