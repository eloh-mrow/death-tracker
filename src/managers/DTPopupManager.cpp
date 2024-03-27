#pragma once

#include "DTPopupManager.hpp"

bool DTPopupManager::m_infoAlertOpen = false;
GJGameLevel* DTPopupManager::m_level = nullptr;

bool DTPopupManager::isInfoAlertOpen() {
    return m_infoAlertOpen;
}

void DTPopupManager::setInfoAlertOpen(bool open) {
    m_infoAlertOpen = open;
}


GJGameLevel* DTPopupManager::getCurrentLevel() {
    return m_level;
}

void DTPopupManager::setCurrentLevel(GJGameLevel* level){
    m_level = level;
}