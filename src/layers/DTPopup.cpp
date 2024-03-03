#include "DTPopup.hpp"
#include "../utils/DTPopupManager.hpp"
#include "../utils/SaveManager.hpp"

bool DTPopup::setup(CCSize popupSize) {
	// setup variables
	m_popupSize = popupSize;

	// set title
	auto session = DTPopupManager::showSessionDeaths();
	auto showPassRate = DTPopupManager::showPassRate();
	auto noSessionDeaths = SaveManager::hasNoDeaths(true);

	std::string titleStr = session
		? showPassRate
			? "Session Pass Rate"
			: "Session Deaths"
		: showPassRate
			? "Pass Rate"
			: "Deaths";

	float scale = session
		? noSessionDeaths
			? 0.8f
			: showPassRate
				? 0.6f // change this
				: 0.65f
		: 0.9f;

	this->setTitle(titleStr, "goldFont.fnt", scale);

	// fix some misc default menu customization
	auto titleNode = m_title;
	titleNode->setPositionY(titleNode->getPositionY() - 4);
	m_closeBtn->setVisible(false);
	this->setOpacity(150);

	// calculate pass rates
	m_passRates = {};
	int totalDeaths = 0;
	auto deaths = SaveManager::getDeaths();

	for (auto& count : deaths) {
		totalDeaths += count;
		m_passRates.push_back(0);
	}

	int index = 0;

	int offset = SaveManager::getLevel()->m_normalPercent.value() == 100
		? 1
		: 0;

	for (auto& count : deaths) {
		totalDeaths -= count;

		float passCount = totalDeaths;
		m_passRates[index] = (passCount + offset) / (passCount + count + offset) * 100;

		index++;
	}

	this->setupPopupPages();
	log::info("touch priority");
	handleTouchPriority(this);
	return true;
}

void DTPopup::createNewPageLayer() {
	if (m_pageLayer != nullptr)
		this->removeChild(m_pageLayer);

	m_pageLayer = CCLayer::create();
	this->addChild(m_pageLayer);
}

void DTPopup::setupPopupPages() {
	auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();

	// show popup if no deaths recorded for this level
	if (SaveManager::hasNoDeaths(true)) {
		this->showNoDeathsPage();
		return;
	}

	// create pages
	this->createPages();

	// add back button
	auto backBtnMenu = CCMenu::create();

	backBtnMenu->setPosition({
		24.f,
		WIN_SIZE.height - 23
	});

	auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
		this,
		menu_selector(DTPopup::onClose)
	);

	backBtnMenu->addChild(backBtn);
	this->addChild(backBtnMenu);

	// show current page
	this->showPage();
}

// this is probably the worst way ever to do this
// but idk what the fuck im doing :skull:
void DTPopup::showNoDeathsPage() {
	auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();
	this->createNewPageLayer();

	// add labels
	auto labelsContainer = CCNode::create();
	labelsContainer->setAnchorPoint({0.5f, 0.5f});

	labelsContainer->setPosition({
		WIN_SIZE.width / 2,
		(WIN_SIZE.height / 2) + 7
	});

	labelsContainer->setContentSize({
		m_popupSize.width,
		m_popupSize.height
	});

	labelsContainer->setScale(0.45f);

	auto label1 = CCLabelBMFont::create("No deaths recorded.", "bigFont.fnt");
	auto label2 = CCLabelBMFont::create("Start playing the level", "bigFont.fnt");
	auto label3 = CCLabelBMFont::create("to record your deaths!", "bigFont.fnt");

	labelsContainer->addChild(label1);
	labelsContainer->addChild(label2);
	labelsContainer->addChild(label3);

	auto labelsLayout = ColumnLayout::create();
	labelsLayout->setAxisReverse(true);
	labelsContainer->setLayout(labelsLayout);

	m_pageLayer->addChild(labelsContainer);

	// add OK button
	auto okBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("OK"),
		this,
		menu_selector(DTPopup::onClose)
	);

	auto menu = CCMenu::create();
	auto btnSize = okBtn->getContentSize();

	menu->setPosition({
		WIN_SIZE.width / 2,
		(WIN_SIZE.height / 2) - (m_popupSize.height / 2) + (btnSize.height)
	});

	menu->setContentSize({m_popupSize.width, 0});
	menu->addChild(okBtn);

	// add session button
	if (DTPopupManager::showSessionDeaths()) {
		auto sessionBtnSpr = ButtonSprite::create("", 15.f, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);
		auto sessionIconSpr = CCSprite::create("dt_sessionIcon_active.png"_spr);

		auto sessionBtnSprSize = sessionBtnSpr->m_BGSprite->getContentSize();
		sessionIconSpr->setScale(0.8f);

		sessionIconSpr->setPosition({
			sessionBtnSprSize.width / 2,
			sessionBtnSprSize.height / 2
		});

		sessionBtnSpr->addChild(sessionIconSpr);

		auto sessionBtn = CCMenuItemSpriteExtra::create(
			sessionBtnSpr,
			this,
			menu_selector(DTPopup::onToggleSessionDeaths)
		);

		menu->addChild(sessionBtn);
	}

	auto menuLayout = RowLayout::create();
	menuLayout->setGap(8.f);
	menu->setLayout(menuLayout);

	m_pageLayer->addChild(menu);
}

void DTPopup::createPages() {
	auto deaths = SaveManager::getDeaths();

	int i = 0;
	int pageLen = SaveManager::isPlatformer() ? 10 : 13;
	int curPageLen = 0;
	int curPage = 0;

	for (auto& count : deaths) {
		int percent = i++;
		if (!count) continue;

		// add to curPage
		if (!curPageLen++) m_pages.push_back({});
		m_pages[curPage].push_back(percent);

		// go to next page
		if (curPageLen == pageLen) {
			curPageLen = 0;
			curPage++;
		}
	}
}

void DTPopup::nextPage(CCObject* sender) {
	if (m_pageIndex == m_pages.size() - 1) return;
	m_pageIndex++;
	showPage();
}

void DTPopup::backPage(CCObject* sender) {
	if (!m_pageIndex) return;
	m_pageIndex--;
	showPage();
}

void DTPopup::onCopy(CCObject* sender) {
	try {
		std::stringstream ss;

		auto deaths = SaveManager::getDeaths();
		int i = 0;

		if (DTPopupManager::showSessionDeaths()) {
			if (DTPopupManager::showPassRate())
				ss << "Session Pass Rate:";

			else ss << "Session Deaths:";
		}

		else if (DTPopupManager::showPassRate()) ss << "Pass Rate:";
		else ss << "Deaths:";

		ss << std::endl;

		for (auto& count : deaths) {
			int percent = i++;
			if (!count) continue;

			// platformer deaths
			if (SaveManager::isPlatformer()) {
				if (DTPopupManager::showPassRate()) ss << std::format("checkpoint {}: {:.2f}%", percent, m_passRates[percent]);
				else ss << std::format("checkpoint {}: x{}", percent, count);
				if (SaveManager::isNewBest(percent)) ss << " (new best)";
				ss << std::endl;

				continue;
			}

			// normal mode deaths
			if (!DTPopupManager::showPassRate()) {
				ss << std::format("{}%x{}", percent, count);
				if (SaveManager::isNewBest(percent)) ss << " (new best)";
				ss << std::endl;

				continue;
			}

			// normal mode pass rate
			ss << std::format("{}%: {:.2f}%", percent, m_passRates[percent]);
			if (SaveManager::isNewBest(percent)) ss << " (new best)";
			ss << std::endl;
		}

		clipboard::write(ss.str());

		Notification::create(
			std::string("Copied successfully"),
			CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png")
		)->show();
	} catch (const std::exception& e) {
		Notification::create(
			std::string("Failed to copy"),
			CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png")
		)->show();
	}
}

void DTPopup::onToggleSessionDeaths(CCObject* sender) {
	DTPopupManager::toggleShowSessionDeaths();

	// re-create popup
	auto parent = this->getParent();
	this->removeFromParent();
	parent->addChild(DTPopup::create());
}

void DTPopup::onTogglePassRate(CCObject* sender) {
	DTPopupManager::toggleShowPassRate();

	// re-create popup
	auto parent = this->getParent();
	this->removeFromParent();
	parent->addChild(DTPopup::create());
}

void DTPopup::showPage() {
	auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();
	this->createNewPageLayer();

	int heightTopOffset = 40;
	int padding = 19;

	// create copy button
	auto cpyBtnSpr = ButtonSprite::create("COPY");
	cpyBtnSpr->setScale(0.7f);

	auto cpyBtn = CCMenuItemSpriteExtra::create(
		cpyBtnSpr,
		this,
		menu_selector(DTPopup::onCopy)
	);

	auto cpyBtnSize = cpyBtn->getContentSize();

	// create session button
	auto sessionBtnSpr = ButtonSprite::create("", 15.f, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);

	auto sessionIconSpr = CCSprite::create(
		DTPopupManager::showSessionDeaths()
			? "dt_sessionIcon_active.png"_spr
			: "dt_sessionIcon_inactive.png"_spr
	);

	auto sessionBtnSprSize = sessionBtnSpr->m_BGSprite->getContentSize();
	sessionIconSpr->setScale(0.8f);

	sessionIconSpr->setPosition({
		sessionBtnSprSize.width / 2,
		sessionBtnSprSize.height / 2
	});

	sessionBtnSpr->addChild(sessionIconSpr);
	sessionBtnSpr->setScale(0.7f);

	auto sessionBtn = CCMenuItemSpriteExtra::create(
		sessionBtnSpr,
		this,
		menu_selector(DTPopup::onToggleSessionDeaths)
	);

	// create pass rate button
	auto passRateBtnSpr = ButtonSprite::create("", 15.f, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);

	auto passRateIconSpr = CCSprite::create(
		DTPopupManager::showPassRate()
			? "dt_passRateIcon_active.png"_spr
			: "dt_passRateIcon_inactive.png"_spr
	);

	auto passRateBtnSize = passRateBtnSpr->m_BGSprite->getContentSize();
	passRateIconSpr->setScale(0.8f);

	passRateIconSpr->setPosition({
		passRateBtnSize.width / 2,
		passRateBtnSize.height / 2
	});

	passRateBtnSpr->addChild(passRateIconSpr);
	passRateBtnSpr->setScale(0.7f);

	auto passRateBtn = CCMenuItemSpriteExtra::create(
		passRateBtnSpr,
		this,
		menu_selector(DTPopup::onTogglePassRate)
	);

	auto btnsMenu = CCMenu::create();
	btnsMenu->addChild(cpyBtn);
	btnsMenu->addChild(sessionBtn);
	btnsMenu->addChild(passRateBtn);
	btnsMenu->setContentSize({m_popupSize.width, 0.f});

	btnsMenu->setPosition({
		WIN_SIZE.width / 2,
		(WIN_SIZE.height / 2) - (m_popupSize.height / 2) + (cpyBtnSize.height)
	});

	auto btnsMenuLayout = RowLayout::create();
	btnsMenuLayout->setGap(7.f);
	btnsMenu->setLayout(btnsMenuLayout);

	m_pageLayer->addChild(btnsMenu);

	// show next button
	if (m_pageIndex < m_pages.size() - 1) {
		auto nxtBtn = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
			this,
			menu_selector(DTPopup::nextPage)
		);

		auto nxtBtnMenu = CCMenu::create();

		nxtBtnMenu->setPosition({
			(WIN_SIZE.width / 2) + (m_popupSize.width / 2) + padding,
			WIN_SIZE.height / 2
		});

		nxtBtnMenu->setScaleX(-0.8f);
		nxtBtnMenu->setScaleY(0.8f);
		nxtBtnMenu->setAnchorPoint({0.f, 0.f});
		nxtBtnMenu->addChild(nxtBtn);
		m_pageLayer->addChild(nxtBtnMenu);
	}

	// show back button
	if (m_pageIndex > 0) {
		auto bckBtn = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
			this,
			menu_selector(DTPopup::backPage)
		);

		auto bckBtnMenu = CCMenu::create();

		bckBtnMenu->setPosition({
			(WIN_SIZE.width / 2) - (m_popupSize.width / 2) - padding,
			WIN_SIZE.height / 2
		});

		bckBtnMenu->setScale(0.8f);
		bckBtnMenu->setAnchorPoint({0.f, 0.f});
		bckBtnMenu->addChild(bckBtn);
		m_pageLayer->addChild(bckBtnMenu);
	}

	// show page deaths
	auto layout = ColumnLayout::create();
	layout->setAxisReverse(true);
	layout->setAxisAlignment(AxisAlignment::End);
	layout->setGap(SaveManager::isPlatformer() ? 6.f : 3.f);

	auto deathsNode = CCNode::create();

	deathsNode->setPosition({
		WIN_SIZE.width / 2,
		(WIN_SIZE.height / 2) + (m_popupSize.height / 2) - heightTopOffset
	});

	deathsNode->setContentSize({
		m_popupSize.width - (padding * 2),
		m_popupSize.height - heightTopOffset - cpyBtnSize.height - padding + 5
	});

	deathsNode->setAnchorPoint({0.5f, 1.f});
	deathsNode->setLayout(layout);

	auto deaths = SaveManager::getDeaths();

	for (auto& percent : m_pages[m_pageIndex]) {
		int count = deaths[percent];

		if (SaveManager::isPlatformer()) {
			auto checkpt = std::to_string(percent);

			auto countStr = DTPopupManager::showPassRate()
				? std::format("{:.2f}%", m_passRates[percent])
				: std::format("x{}", count);

			auto checkptSpr = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
			checkptSpr->setScale(0.65f);

			auto checkptLbl = CCLabelBMFont::create(checkpt.c_str(), "bigFont.fnt");
			checkptLbl->setZOrder(2);
			checkptLbl->setScale(0.35f);

			auto checkptLblShadow = CCLabelBMFont::create(checkpt.c_str(), "bigFont.fnt");
			checkptLblShadow->setZOrder(1);
			checkptLblShadow->setScale(0.35f);

			auto checkptLblPos = checkptLbl->getPosition();
			checkptLblShadow->setPosition({
				checkptLblPos.x + 0.5f,
				checkptLblPos.y - 0.5f
			});

			checkptLblShadow->setColor({0, 0, 0});
			checkptLblShadow->setOpacity(120);

			auto countLbl = CCLabelBMFont::create(countStr.c_str(), "chatFont.fnt");
			countLbl->setAnchorPoint({0.f, 0.5f});

			// new bests are yellow
			if (SaveManager::isNewBest(percent))
				countLbl->setColor({255, 255, 0});

			auto checkptNode = CCNode::create();
			checkptNode->addChild(checkptSpr);
			checkptNode->addChild(checkptLbl);
			checkptNode->addChild(checkptLblShadow);
			checkptNode->setAnchorPoint({0.5f, 0.5f});

			checkptNode->setContentSize({
				(checkptLbl->getContentSize().width * checkptLbl->getScale()),
				0
			});

			auto node = CCNode::create();
			node->setAnchorPoint({0.5f, 0.5f});

			node->setContentSize({
				m_popupSize.width - (padding * 2),
				0
			});

			node->addChild(checkptNode);
			node->addChild(countLbl);
			deathsNode->addChild(node);

			auto layout = RowLayout::create();
			node->setLayout(layout);
			checkptNode->ignoreAnchorPointForPosition(true); // idk what this does, but it works! :grin:
		} else {
			float passRate = m_passRates[percent];

			auto labelStr = DTPopupManager::showPassRate()
				? std::format("{}% ({:.2f}%)", percent, passRate)
				: std::format("{}% x{}", percent, count);

			auto label = CCLabelBMFont::create(labelStr.c_str(), "chatFont.fnt");

			// new bests are yellow
			if (SaveManager::isNewBest(percent))
				label->setColor({255, 255, 0});

			deathsNode->addChild(label);
		}
	}

	deathsNode->setLayout(layout);
	m_pageLayer->addChild(deathsNode);
	handleTouchPriority(this);
}

void DTPopup::keyDown(cocos2d::enumKeyCodes key) {
	switch (key) {
		case KEY_Left:
		case CONTROLLER_Left: {
			this->backPage(nullptr);
			break;
		}

		case KEY_Right:
		case CONTROLLER_Right: {
			this->nextPage(nullptr);
			break;
		}
	}

	Popup::keyDown(key);
}

void DTPopup::onClose(CCObject* sender) {
	Popup::onClose(sender);
	DTPopupManager::onDTPopupClose();
}

DTPopup* DTPopup::create() {
	auto ret = new DTPopup();
	auto popupSize = DTPopupManager::getSize();

	if (ret && ret->init(popupSize.width, popupSize.height, popupSize, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}