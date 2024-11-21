#pragma once

#include <Geode/Geode.hpp>
#include "DTLayer.hpp"

using namespace geode::prelude;

class DTLevelSpecificSettingsLayer : public CCNode, public FLAlertLayerProtocol, public TextInputDelegate {
    protected:
        bool init(const CCSize& size, DTLayer* const& _DTLayer);
    public:
        static DTLevelSpecificSettingsLayer* create(const CCSize& size, DTLayer* const& _DTLayer);

        CCSize m_Size;

        //runs
        TextInput* m_AddRunAllowedInput;
        CCMenuItemSpriteExtra* AddRunAllowedButton;
        CCMenuItemToggler* allRunsToggle;
        void OnToggleAllRuns(CCObject*);

        CCMenu* runsAllowedCellsCont;
        void addRunAllowed(CCObject*);
        void removeRunAllowed(const int& percent, CCNode* cell);

        void deleteUnused(CCObject*);
        FLAlertLayer* m_RunDeleteAlert;

        void onRunsAInfo(CCObject*);

        CCMenuItemSpriteExtra* DeleteUnusedButton;

        TextInput* HideRunLengthInput;
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
        DTLayer* myDTLayer;
        void EnableTouch(bool b);
        std::vector<CCMenu*> touchMenus{};

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected) override;
        void textChanged(CCTextInputNode* input) override;
        
        void addRunStartInputKeyDown();
};