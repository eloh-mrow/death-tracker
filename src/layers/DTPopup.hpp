#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTPopup : public Popup<CCSize> {
protected:
	CCSize m_popupSize;
	CCNode* m_pageLayer = nullptr;
	int m_pageIndex = 0;
	std::vector<float> m_passRates{};
	std::vector<std::vector<int>> m_pages;

	bool setup(CCSize popupSize) override;
	void createNewPageLayer();
	void setupPopupPages();
	void showNoDeathsPage();
	void createPages();
	void nextPage(CCObject* sender);
	void backPage(CCObject* sender);
	void onCopy(CCObject* sender);
	void onInfo(CCObject* sender);
	void onToggleSessionDeaths(CCObject* sender);
	void onTogglePassRate(CCObject* sender);
	void showPage();
	void keyDown(cocos2d::enumKeyCodes key);
	virtual void onClose(CCObject* sender);

public:
	static DTPopup* create();
};