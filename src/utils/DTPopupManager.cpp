#include "DTPopupManager.hpp"
#include "SaveManager.hpp"

FLAlertLayer* DTPopupManager::m_infoAlert = nullptr;
DTButtonLayer* DTPopupManager::m_dtBtn = nullptr;
bool DTPopupManager::m_isDTBtnEnabled = false;
bool DTPopupManager::m_showSessionDeaths = false;
bool DTPopupManager::m_showPassRate = false;
bool DTPopupManager::m_showInfoAlert = false;

CCSize DTPopupManager::getSize() {
	auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();

	return SaveManager::hasNoDeaths(true)
		? CCSize(240.f, 160.f)
		: CCSize(160.f, WIN_SIZE.height * 0.85);
}

FLAlertLayer* DTPopupManager::getActiveInfoAlert() {
	return m_infoAlert;
}

bool DTPopupManager::isDTBtnEnabled() {
	return m_isDTBtnEnabled;
}

void DTPopupManager::enableDTBtn() {
	m_isDTBtnEnabled = true;
}

void DTPopupManager::toggleShowSessionDeaths() {
	m_showSessionDeaths ^= 1;
}

bool DTPopupManager::showSessionDeaths() {
	return m_showSessionDeaths;
}

void DTPopupManager::toggleShowPassRate() {
	m_showPassRate ^= 1;
}

bool DTPopupManager::showPassRate() {
	return m_showPassRate;
}

void DTPopupManager::onInfoAlertOpen(FLAlertLayer* infoAlert, DTButtonLayer* dtBtn) {
	m_infoAlert = infoAlert;
	m_dtBtn = dtBtn;
	m_isDTBtnEnabled = true;
	handleTouchPriority(m_infoAlert);
}

void DTPopupManager::onInfoAlertClose(bool forceRemove) {
	m_isDTBtnEnabled = false;
	m_infoAlert = nullptr;
	DTPopupManager::removeDTBtn(forceRemove);
	SaveManager::setLevel(nullptr);
}

void DTPopupManager::onDTPopupOpen() {
	if (m_infoAlert == nullptr) return;
	m_infoAlert->setVisible(false);
}

void DTPopupManager::onDTPopupClose() {
	m_showSessionDeaths = false;
	m_showPassRate = false;
	if (m_infoAlert == nullptr) return;
	m_infoAlert->setVisible(true);
	handleTouchPriority(m_infoAlert);
}

void DTPopupManager::removeDTBtn(bool forceRemove) {
	if (forceRemove && m_dtBtn != nullptr) m_dtBtn->removeFromParent();
	m_dtBtn = nullptr;
}

bool DTPopupManager::showInfoAlert() {
	return m_showInfoAlert;
}

void DTPopupManager::setShowInfoAlert(bool show) {
	m_showInfoAlert = show;
}