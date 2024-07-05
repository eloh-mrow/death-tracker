#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class confirmImportLayer : public Popup<DTLayer* const&, Deaths, Runs> {
    protected:
        bool setup(DTLayer* const& layer, Deaths ds, Runs rs) override;
    public:
        static confirmImportLayer* create(DTLayer* const& layer, Deaths ds, Runs rs);

        DTLayer* m_DTLayer;
        Deaths deaths;
        Runs runs;

        void update(float delta);
        CCNode* text;
        
        void yesClicked(CCObject*);
};