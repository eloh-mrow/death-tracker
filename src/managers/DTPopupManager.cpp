#pragma once

#include "DTPopupManager.hpp"

bool DTPopupManager::m_infoAlertOpen = false;

bool DTPopupManager::isInfoAlertOpen() {
    return m_infoAlertOpen;
}

void DTPopupManager::setInfoAlertOpen(bool open) {
    m_infoAlertOpen = open;
}

GJGameLevel* DTPopupManager::m_CurrentLevel{};

GJGameLevel* DTPopupManager::getCurrentLevel(){
    return m_CurrentLevel;
}
void DTPopupManager::setCurrentLevel(GJGameLevel* level){
    m_CurrentLevel = level;
}