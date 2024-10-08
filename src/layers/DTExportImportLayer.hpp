#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class DTExportImportLayer : public Popup<DTLayer*> {
    protected:
        bool setup(DTLayer* layer) override;
    public:
        static DTExportImportLayer* create(DTLayer* layer);

        DTLayer* m_DTLayer;

        //export

        void onExportWithLabelsClicked(CCObject*);
        void onExportClicked(CCObject*);
        void onOpenExportsFolderClicked(CCObject*);

        CCMenuItemToggler* exportWithLabels;
        bool exportWithLabelsB;

        //import

        void onImportClicked(CCObject*);
        LoadingCircle* loading;
        bool importing;
        EventListener<Task<Result<std::filesystem::path>>> openFileLocListener;

        //info
        void onExportInfo(CCObject*);
        void onImportInfo(CCObject*);
};