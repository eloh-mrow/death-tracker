#pragma once

#include <Geode/Geode.hpp>
#include "DTLayer.hpp"

using namespace geode::prelude;

class DTLevelSpecificSettingsLayer : public CCNode, public FLAlertLayerProtocol, public TextInputDelegate {
    protected:
        bool init(const CCSize& size, DTLayer* const& _DTLayer);
    public:
        static DTLevelSpecificSettingsLayer* create(const CCSize& size, DTLayer* const& _DTLayer);

        //enable/disable touch on this layer
        void EnableTouch(bool b);

        CCSize m_Size;

    private:

        //runs
        
        //toggles the 'track all runs' option
        void OnToggleAllRuns(CCObject*);

        //adds the run inputted on the m_AddRunAllowedInput input to the allowed runs
        void addRunAllowed(CCObject*);
        //removes an allowed run from the list
        void removeRunAllowed(const int& percent, CCNode* cell);

        //deletes all runs that dont starts from the allowed runs percents
        void deleteUnused(CCObject*);
        //info about the run managment and options
        void onRunsAInfo(CCObject*);

        TextInput* m_AddRunAllowedInput;
        CCMenuItemSpriteExtra* AddRunAllowedButton;
        CCMenuItemToggler* allRunsToggle;

        CCMenu* runsAllowedCellsCont;
        
        FLAlertLayer* m_RunDeleteAlert;

        CCMenuItemSpriteExtra* DeleteUnusedButton;

        TextInput* HideRunLengthInput;
        TextInput* HideUptoInput;

        //Save Manegement

        //opens the alarm for deletall all of your progress
        void onCurrentDeleteClicked(CCObject*);
        //opens the alarm for reverting your save back to the backup
        void onRevertClicked(CCObject*);
        //opens the export/import layer
        void onExportClicked(CCObject*);
        //info about save management and its options
        void onSaveManagementInfo(CCObject*);

        FLAlertLayer* currDeleteAlert = nullptr;
        FLAlertLayer* revertAlert = nullptr;

        //run management

        //adds a run from 0% depending on the amount
        void onAddedFZRun(CCObject*);
        //removes a run from 0% depending on the amount
        void onRemovedFZRun(CCObject*);

        //adds a run depending on the amount
        void onAddedRun(CCObject*);
        //removes a run depending on the amount
        void onRemovedRun(CCObject*);
        //swaps between the addRunStartInput and addRunEndInput when clicking the - key
        void addRunStartInputKeyDown();

        //info about run managment and the usage of it
        void onModRunsInfo(CCObject*);

        TextInput* addFZRunInput;
        TextInput* addRunStartInput;
        TextInput* addRunEndInput;
        TextInput* runsAmountInput;

        CCLabelBMFont* runSeparator;

        //overall

        void FLAlert_Clicked(FLAlertLayer* layer, bool selected) override;
        void textChanged(CCTextInputNode* input) override;

        DTLayer* myDTLayer;
        std::vector<CCMenu*> touchMenus{};
};