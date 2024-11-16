#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class confirmImportLayer : public Popup<DTLayer* const&, const Deaths&, const Runs&> {
    protected:
        bool setup(DTLayer* const& layer, const Deaths& ds, const Runs& rs) override;
    public:
        static confirmImportLayer* create(DTLayer* const& layer, const Deaths& ds, const Runs& rs);

        DTLayer* m_DTLayer;
        Deaths deaths;
        Runs runs;

        void update(float delta);
        CCNode* text;
        
        void yesClicked(CCObject*);
};