#if defined(GEODE_IS_MACOS)

#include "../hooks/DTCCMenuItemSpriteExtra.hpp"

void DTCCMenuItemSpriteExtra::activate(){
    CCMenuItemSpriteExtra::activate();

    if (getID() == "retry-button")
        m_fields->didActivate = true;
}

#endif