#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class DTExportImportLayer : public Popup<DTLayer* const&> {
    protected:
        bool setup(DTLayer* const& layer) override;
    public:
        static DTExportImportLayer* create(DTLayer* const& layer);
        
    private:

        //export

        //toggles the setting "export with layers"
        void onExportWithLabelsClicked(CCObject*);
        //exports the current levels runs into a text file
        void onExportClicked(CCObject*);
        //opens the default exports folder in the file explorer
        void onOpenExportsFolderClicked(CCObject*);

        CCMenuItemToggler* exportWithLabels;
        bool exportWithLabelsB;

        //import

        //opens the file select menu for importing a runs text file, will open the 'confirmImportLayer' if successful
        void onImportClicked(CCObject*);

        //reads runs data from a long string
        std::pair<const Deaths&, const Runs&> readRunsFromText(const std::string& textToRead);

        LoadingCircle* loading;
        bool importing;
        EventListener<Task<Result<std::filesystem::path>>> openFileLocListener;

        //info

        //info about exporting data
        void onExportInfo(CCObject*);
        //info about importing data
        void onImportInfo(CCObject*);

        DTLayer* m_DTLayer;
};