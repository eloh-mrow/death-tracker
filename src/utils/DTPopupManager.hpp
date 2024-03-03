#pragma once

#include <Geode/Geode.hpp>
#include "../layers/DTButtonLayer.hpp"

using namespace geode::prelude;

class DTPopupManager {
private:
	static FLAlertLayer* m_infoAlert;
	static DTButtonLayer* m_dtBtn;
	static bool m_isDTBtnEnabled;
	static bool m_showSessionDeaths;
	static bool m_showPassRate;

public:
	DTPopupManager() = delete;
	static CCSize getSize();
	static FLAlertLayer* getActiveInfoAlert();
	static bool isDTBtnEnabled();
	static void enableDTBtn();
	static void toggleShowSessionDeaths();
	static bool showSessionDeaths();
	static void toggleShowPassRate();
	static bool showPassRate();
	static void onInfoAlertOpen(FLAlertLayer* infoAlert, DTButtonLayer* dtBtn);
	static void onInfoAlertClose(bool forceRemove = false);
	static void onDTPopupOpen();
	static void onDTPopupClose();
	static void removeDTBtn(bool forceRemove = false);
};