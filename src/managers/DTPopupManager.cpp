#pragma once

#include "DTPopupManager.hpp"

bool DTPopupManager::m_infoAlertOpen = false;

bool DTPopupManager::isInfoAlertOpen() {
    return m_infoAlertOpen;
}

void DTPopupManager::setInfoAlertOpen(bool open) {
    m_infoAlertOpen = open;
}