#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTLevelSpecificSettingsLayer : public CCNode, public FLAlertLayerProtocol, public TextInputDelegate {
    protected:
        bool init(CCSize size, CCNode* _DTLayer);
    public:
        static DTLevelSpecificSettingsLayer* create(CCSize size, CCNode* _DTLayer);

        //runs
        TextInput* m_AddRunAllowedInput;
        CCMenuItemSpriteExtra* AddRunAllowedButton;
        CCMenuItemToggler* allRunsToggle;
        void OnToggleAllRuns(CCObject*);

        CCMenu* runsAllowedCellsCont;
        void addRunAllowed(CCObject*);
        void removeRunAllowed(int precent, CCNode* cell);

        void deleteUnused(CCObject*);
        FLAlertLayer* m_RunDeleteAlert;

        void onRunsAInfo(CCObject*);

        CCMenuItemSpriteExtra* DeleteUnusedButton;

        TextInput* HideUptoInput;

        //Save Manegment
        void onCurrentDeleteClicked(CCObject*);
        FLAlertLayer* currDeleteAlert = nullptr;

        void onRevertClicked(CCObject*);
        FLAlertLayer* revertAlert = nullptr;

        void onExportClicked(CCObject*);

        void onSaveManagementInfo(CCObject*);

        //run management
        TextInput* addFZRunInput;
        TextInput* addRunStartInput;
        TextInput* addRunEndInput;
        TextInput* runsAmountInput;
        void onAddedFZRun(CCObject*);
        void onRemovedFZRun(CCObject*);

        void onAddedRun(CCObject*);
        void onRemovedRun(CCObject*);

        void onModRunsInfo(CCObject*);

        CCLabelBMFont* runSeparator;

        //overall
        CCNode* myDTLayer;
        void EnableTouch(bool b);
        std::vector<CCMenu*> touchMenus{};

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected);
        void textChanged(CCTextInputNode* input);
        
        void addRunStartInputKeyDown();
};