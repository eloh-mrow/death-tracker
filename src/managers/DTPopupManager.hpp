#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTPopupManager {
private:
    static bool m_infoAlertOpen;
    static CCMenu* m_dtBtnMenu;

public:
    static bool isInfoAlertOpen();
    static void setInfoAlertOpen(bool open);
};