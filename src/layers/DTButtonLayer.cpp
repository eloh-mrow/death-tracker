#include "DTButtonLayer.hpp"
#include "DTPopup.hpp"
#include "../utils/DTPopupManager.hpp"
#include "../utils/SaveManager.hpp"

bool DTButtonLayer::init() {
	if (!CCLayer::init()) return false;

	auto btnSprite = CCSprite::create("dt_skullBtn.png"_spr);
	btnSprite->setScale(0.25f);

	auto btn = CCMenuItemSpriteExtra::create(
		btnSprite,
		this,
		menu_selector(DTButtonLayer::onOpenDTPopup)
	);

	auto btnSize = btn->getContentSize();

	btn->setPosition({
		-(btnSize.width - 2),
		btnSize.height - 2.5f
	});

	auto menu = CCMenu::create();
	menu->addChild(btn);
	this->addChild(menu);

	return true;
}

DTButtonLayer* DTButtonLayer::create() {
	auto ret = new DTButtonLayer();

	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

void DTButtonLayer::onOpenDTPopup(CCObject* sender) {
	// prevent crashes
	if (
		DTPopupManager::getActiveInfoAlert() == nullptr
		|| SaveManager::getLevel() == nullptr
	) return;

	DTPopupManager::onDTPopupOpen();

	auto scene = CCDirector::sharedDirector()->getRunningScene();
	scene->addChild(DTPopup::create());
}