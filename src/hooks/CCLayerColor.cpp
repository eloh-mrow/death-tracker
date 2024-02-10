#include <Geode/modify/CCLayerColor.hpp>
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(CCLayerColor) {
	bool initWithColor(const ccColor4B& color) {
		if (!CCLayerColor::initWithColor(color)) return false;

		// remove the m_dtBtn as a fail safe
		// this is guaranteed to fire before FLAlertLayer::init()
		DTPopupManager::removeDTBtn();
		return true;
	}
};