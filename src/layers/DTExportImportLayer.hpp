#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class DTExportImportLayer : public Popup<DTLayer* const&>, public TextInputDelegate {
    protected:
        bool setup(DTLayer* const& layer) override;
    public:
        static DTExportImportLayer* create(DTLayer* const& layer);

        DTLayer* m_DTLayer;

        //export

        void onExportWithLabelsClicked(CCObject*);
        void onExportClicked(CCObject*);
        void onOpenExportsFolderClicked(CCObject*);

        CCMenuItemToggler* exportWithLabels;
        bool exportWithLabelsB;
};