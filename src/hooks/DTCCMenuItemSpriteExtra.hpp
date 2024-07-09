#if defined(GEODE_IS_MACOS)

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

class $modify(DTCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
    public:
        struct Fields {
            bool didActivate = false;
        };

        void activate();

};

#endif