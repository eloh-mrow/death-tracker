#include "DTPopupManager.hpp"

bool DTPopupManager::m_infoAlertOpen = false;
GJGameLevel* DTPopupManager::m_level = nullptr;

bool DTPopupManager::isInfoAlertOpen() {
    return m_infoAlertOpen;
}

void DTPopupManager::setInfoAlertOpen(const bool& open) {
    m_infoAlertOpen = open;
}


GJGameLevel* DTPopupManager::getCurrentLevel() {
    return m_level;
}

void DTPopupManager::setCurrentLevel(GJGameLevel* const& level){
    m_level = level;
}  