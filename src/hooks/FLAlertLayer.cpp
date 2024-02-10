#include <Geode/modify/FLAlertLayer.hpp>
#include "../utils/DTPopupManager.hpp"

using namespace geode::prelude;

class $modify(DTAlertLayer, FLAlertLayer) {
	bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
		if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8))
			return false;

		if (DTPopupManager::isDTBtnEnabled()) {
			auto alertBgSize = getChildOfType<CCScale9Sprite>(this->m_mainLayer, 0)->getContentSize();

			auto dtBtn = DTButtonLayer::create();
			dtBtn->setZOrder(100);

			dtBtn->setPosition({
				alertBgSize.width / 2,
				-alertBgSize.height / 2
			});

			this->m_mainLayer->addChild(dtBtn);
			DTPopupManager::onInfoAlertOpen(this, dtBtn);
		}

		return true;
	}

	// manually hide the DT button
	// if cvolton.betterinfo is loaded
	void show() {
		FLAlertLayer::show();

		if (!DTPopupManager::isDTBtnEnabled()) return;
		if (!Loader::get()->isModLoaded("cvolton.betterinfo")) return;
		if (this->getChildByIDRecursive("cvolton.betterinfo/next-button") != nullptr) return;
		DTPopupManager::onInfoAlertClose(true);
	}

	void onBtn1(CCObject* sender) {
		if (this->m_button1->getParent() == sender)
			DTPopupManager::onInfoAlertClose();

		FLAlertLayer::onBtn1(sender);
	}

	void keyBackClicked() {
		DTPopupManager::onInfoAlertClose();
		FLAlertLayer::keyBackClicked();
	}
};