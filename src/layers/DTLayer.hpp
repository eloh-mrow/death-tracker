#pragma once

#include <Geode/Geode.hpp>
#include "Geode/ui/TextArea.hpp"
#include "../managers/StatsManager.hpp"
#include "../utils/Save.hpp"

using namespace geode::prelude;

class DTLayer : public Popup<GJGameLevel* const&>, public TextInputDelegate, public FLAlertLayerProtocol, public ColorPickPopupDelegate {
    protected:
        bool setup(GJGameLevel* const& Level) override;

        void update(float delta) override;

        void updateColor(cocos2d::ccColor4B const& color) override;
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
        CCNode* alignmentNode;

        //text
        void RefreshText(bool moveToTop = false);
        CCNode* m_TextCont = nullptr;
        std::string modifyString(std::string ToModify);
        bool isKeyInIndex(const std::string& s, int Index, const std::string& key);

        Result<std::vector<DeathInfo>> CreateDeathsString(const Deaths& deaths, const NewBests& newBests);
        Result<std::vector<DeathInfo>> CreateRunsString(const Runs& runs);
        void refreshStrings();

        std::vector<DeathInfo> m_DeathsInfo;
        std::vector<DeathInfo> selectedSessionInfo;
        std::vector<DeathInfo> m_RunInfo;
        std::vector<DeathInfo> m_SelectedSessionRunInfo;

        std::string deathsString;
        std::string selectedSessionString;

        std::string RunString;
        std::string selectedSessionRunString;

        //session selection
        TextInput* m_SessionSelectionInput = nullptr;
        CCMenu* m_SessionSelectMenu;
        int m_SessionsAmount;
        int m_SessionSelected;
        bool m_SessionSelectionInputSelected;
        void updateSessionString(const int& session);
        void SwitchSessionRight(CCObject*);
        void SwitchSessionLeft(CCObject*);

        //linking
        void OnLinkButtonClicked(CCObject*);
        CCMenuItemSpriteExtra* LinkLevelsButton;
        void UpdateSharedStats();

        //edit layout mode
        CCMenuItemSpriteExtra* m_EditLayoutBtn;
        void onEditLayout(CCObject*);
        CCMenu* m_EditLayoutMenu;
        CCSprite* m_BlackSquare;
        void onEditLayoutApply(CCObject*);
        void EditLayoutEnabled(const bool& b);
        std::vector<CCNode*> m_LayoutLines;
        CCNode* m_LayoutStuffCont = nullptr;
        void createLayoutBlocks();
        bool m_IsMovingAWindow;
        void changeScrollSizeByBoxes(const bool& moveToTop = false);
        void addBox(CCObject*);
        CCMenuItemSpriteExtra* editLayoutApplyBtn;
        CCMenuItemSpriteExtra* addWindowButton;
        CCMenuItemSpriteExtra* layoutInfoButton;
        void clickedWindow(CCNode* window);
        FLAlertLayer* ResetLayoutAlert;
        CCMenuItemSpriteExtra* resetLayoutButton;
        void onResetLayout(CCObject*);
        ColorChannelSprite* colorSpritenb;
        geode::ColorPickPopup* colorSelectnb;
        void editnbcColor(CCObject*);
        ColorChannelSprite* colorSpritesb;
        geode::ColorPickPopup* colorSelectsb;
        void editsbcColor(CCObject*);
        bool openednbLast;

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

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected) override;

        void onClose(cocos2d::CCObject*) override;

        void updateRunsAllowed();

        //graph
        void openGraphMenu(CCObject*);

        //settings
        void onSettings(CCObject*);
        CCMenuItemSpriteExtra* settingsButton;

        //info
        void onLayoutInfo(CCObject*);
        void onCopyInfo(CCObject*);

        //copy
        void copyText(CCObject*);
        bool isInCopyMenu;
        CCMenuItemSpriteExtra* copyInfoButton;
        CCMenuItemSpriteExtra* copyTextButton;

        //specific settings
        void onSpecificSettings(CCObject*);
        CCNode* LevelSpecificSettingsLayer = nullptr;
        bool runningMoveTransition = false;
        void onMoveTransitionEnded(CCObject* LSSL);
        bool isExitingSSLayer = false;
        CCSprite* levelSettingsBSArrow;
};