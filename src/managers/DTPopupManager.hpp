#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTPopupManager {
private:
    static bool m_infoAlertOpen;
    static CCMenu* m_dtBtnMenu;
    static GJGameLevel* m_level;

public:
    static bool isInfoAlertOpen();
    static void setInfoAlertOpen(bool open);

    static GJGameLevel* getCurrentLevel();
    static void setCurrentLevel(GJGameLevel* level);
};