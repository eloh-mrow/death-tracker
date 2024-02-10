// #include <map>
// #include <format>

// #include <Geode/Geode.hpp>
// #include <Geode/modify/GJBaseGameLayer.hpp>
// #include <Geode/modify/PlayLayer.hpp>
// #include <Geode/modify/PlayerObject.hpp>
// #include <Geode/modify/LevelInfoLayer.hpp>
// #include <Geode/modify/EditLevelLayer.hpp>
// #include <Geode/modify/LevelPage.hpp>
// #include <Geode/modify/FLAlertLayer.hpp>
// #include <Geode/modify/CCLayerColor.hpp>
// #include <matjson/stl_serialize.hpp>

// using namespace geode::prelude;
// typedef std::vector<int> Deaths;
// typedef std::map<int, bool> Progresses;

// class DTButtonLayer : public CCLayer {
// protected:
// 	bool init();

// public:
// 	static DTButtonLayer* create();
// 	void onOpenDTPopup(CCObject* sender);
// };

// class DTPopup : public Popup<CCSize> {
// protected:
// 	CCSize m_popupSize;
// 	CCNode* m_pageLayer = nullptr;
// 	int m_pageIndex = 0;
// 	std::vector<std::vector<int>> m_pages;

// 	bool setup(CCSize popupSize) override;
// 	void createNewPageLayer();
// 	void setupPopupPages();
// 	void showNoDeathsPage();
// 	void createPages();
// 	void nextPage(CCObject* sender);
// 	void backPage(CCObject* sender);
// 	void onCopy(CCObject* sender);
// 	void onToggleSessionDeaths(CCObject* sender);
// 	void showPage();
// 	void keyDown(cocos2d::enumKeyCodes key);
// 	virtual void onClose(CCObject* sender);

// public:
// 	static DTPopup* create();
// };

// class DTPopupManager {
// private:
// 	static FLAlertLayer* m_infoAlert;
// 	static DTButtonLayer* m_dtBtn;
// 	static bool m_isDTBtnEnabled;
// 	static bool m_showSessionDeaths;

// public:
// 	DTPopupManager() = delete;
// 	static CCSize getSize();
// 	static FLAlertLayer* getActiveInfoAlert();
// 	static bool isDTBtnEnabled();
// 	static void enableDTBtn();
// 	static void toggleShowSessionDeaths();
// 	static bool showSessionDeaths();
// 	static void onInfoAlertOpen(FLAlertLayer* infoAlert, DTButtonLayer* dtBtn);
// 	static void onInfoAlertClose(bool forceRemove = false);
// 	static void onDTPopupOpen();
// 	static void onDTPopupClose();
// 	static void removeDTBtn(bool forceRemove = false);
// };

// class SaveManager {
// private:
// 	static GJGameLevel* m_level;
// 	static int m_levelCount;
// 	static Deaths m_deaths;
// 	static Deaths m_sessionDeaths;
// 	static Progresses m_progresses;
// 	static int m_checkpoint;
// 	static bool m_usingStartpos;
// 	static bool m_shouldResetSessionDeaths;

// 	static void createBackup();
// 	static std::string getLevelId();
// 	static void calcDeathsAndProgresses();

// public:
// 	SaveManager() = delete;

// 	static GJGameLevel* getLevel();
// 	static void setLevel(GJGameLevel* level);
// 	static bool isPlatformer();
// 	static bool isNewBest(int percent);
// 	static void incLevelCount();
// 	static void setShouldResetSessionDeaths();
// 	static void resetSessionDeaths();
// 	static Deaths getDeaths();
// 	static bool hasNoDeaths(bool checkSessionDeaths = false);
// 	static void addDeath(int percent = 0);
// 	static void allocateDeathsForCheckpoint();
// 	static void incCheckpoint();
// 	static void resetCheckpoint();
// 	static bool isUsingStartpos();
// 	static void setUsingStartpos(bool usingStartpos);
// };

// // globals
// // ----------------------------
// auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();
// std::string LEVEL_COUNT_KEY = "levelCount";

// // SaveManager - helps with save data
// // ----------------------------
// // TODO: add backups
// void SaveManager::createBackup() {}

// std::string SaveManager::getLevelId() {
// 	if (m_level == nullptr) return "";

// 	auto levelId = std::to_string(m_level->m_levelID.value());

// 	// local level postfix
// 	if (m_level->m_levelType != GJLevelType::Saved)
// 		levelId += "-local";

// 	// daily/weekly postfix
// 	if (m_level->m_dailyID > 0)
// 		levelId += "-daily";

// 	// gauntlet level postfix
// 	if (m_level->m_gauntletLevel)
// 		levelId += "-gauntlet";

// 	return levelId;
// }

// void SaveManager::calcDeathsAndProgresses() {
// 	if (m_level == nullptr) return;

// 	// calculate deaths
// 	auto levelId = SaveManager::getLevelId();
// 	auto deaths = Mod::get()->getSavedValue<Deaths>(levelId);
// 	auto sessionDeaths = Mod::get()->getSavedValue<Deaths>(levelId + "-session");

// 	m_deaths = deaths;
// 	m_sessionDeaths = sessionDeaths;

// 	// platformer should start empty
// 	if (SaveManager::isPlatformer()) return;

// 	// calculate progresses
// 	// derived from cvolton.betterinfo
// 	std::string personalBests = m_level->m_personalBests;
// 	int percentage = m_level->m_newNormalPercent2;

// 	Progresses progresses{};
// 	std::stringstream bestsStream(personalBests);
// 	std::string currentBest;
// 	int currentPercent = 0;

// 	while (getline(bestsStream, currentBest, ',')) {
// 		currentPercent += std::stoi(currentBest);
// 		progresses[currentPercent] = true;
// 	}

// 	// default deaths to progresses x1
// 	auto pbs = std::string(m_level->m_personalBests);

// 	if (deaths.empty() && pbs != "" && pbs != "100") {
// 		deaths = Deaths(100);

// 		for (auto& [percent, _] : progresses) {
// 			if (percent == 100) continue; // can't die at 100%
// 			deaths[percent] = 1;
// 		}
// 	}

// 	m_progresses = progresses;
// 	m_deaths = deaths;
// }

// GJGameLevel* SaveManager::getLevel() {
// 	return m_level;
// }

// void SaveManager::setLevel(GJGameLevel* level) {
// 	m_level = level;

// 	if (level != nullptr) {
// 		SaveManager::calcDeathsAndProgresses();
// 	} else {
// 		m_progresses = Progresses();
// 		m_deaths = Deaths();
// 		m_sessionDeaths = Deaths();
// 	}
// }

// bool SaveManager::isPlatformer() {
// 	if (m_level == nullptr) return false;
// 	return m_level->isPlatformer();
// }

// bool SaveManager::isNewBest(int percent) {
// 	return m_progresses[percent];
// }

// void SaveManager::incLevelCount() {
// 	Mod::get()->setSavedValue(LEVEL_COUNT_KEY, ++m_levelCount);

// 	// create new backup
// 	// every 50 levels
// 	if (m_levelCount % 50 == 0)
// 		SaveManager::createBackup();
// }

// void SaveManager::setShouldResetSessionDeaths() {
// 	m_shouldResetSessionDeaths = true;
// }

// void SaveManager::resetSessionDeaths() {
// 	m_shouldResetSessionDeaths = false;
// 	if (m_level == nullptr) return;

// 	m_sessionDeaths = Deaths();
// 	auto levelId = SaveManager::getLevelId();
// 	Mod::get()->setSavedValue(levelId + "-session", m_sessionDeaths);
// }

// Deaths SaveManager::getDeaths() {
// 	return DTPopupManager::showSessionDeaths()
// 		? m_sessionDeaths
// 		: m_deaths;
// }

// bool SaveManager::hasNoDeaths(bool checkSessionDeaths) {
// 	if (m_level == nullptr) return true;

// 	return checkSessionDeaths && DTPopupManager::showSessionDeaths()
// 		? m_sessionDeaths.empty()
// 		: m_deaths.empty();
// }

// void SaveManager::addDeath(int percent) {
// 	// new unplayed level
// 	if (m_deaths.empty()) {
// 		SaveManager::incLevelCount();

// 		if (!SaveManager::isPlatformer())
// 			m_deaths = Deaths(100);
// 	}

// 	// reset prev session
// 	if (m_shouldResetSessionDeaths)
// 		SaveManager::resetSessionDeaths();

// 	// no prev session
// 	if (m_sessionDeaths.empty() && !SaveManager::isPlatformer())
// 		m_sessionDeaths = Deaths(100);

// 	if (SaveManager::isPlatformer()) {
// 		SaveManager::allocateDeathsForCheckpoint();
// 		m_deaths[m_checkpoint]++;
// 		m_sessionDeaths[m_checkpoint]++;
// 	} else {
// 		m_deaths[percent]++;
// 		m_sessionDeaths[percent]++;
// 	}

// 	auto levelId = SaveManager::getLevelId();
// 	Mod::get()->setSavedValue(levelId, m_deaths);
// 	Mod::get()->setSavedValue(levelId + "-session", m_sessionDeaths);
// }

// void SaveManager::allocateDeathsForCheckpoint() {
// 	if (m_level == nullptr) return;

// 	while (m_checkpoint >= m_deaths.size()) {
// 		m_deaths.push_back(0);
// 		m_sessionDeaths.push_back(0);
// 	}
// }

// void SaveManager::incCheckpoint() {
// 	m_checkpoint++;
// }

// void SaveManager::resetCheckpoint() {
// 	m_checkpoint = 0;
// }

// bool SaveManager::isUsingStartpos() {
// 	return m_usingStartpos;
// }

// void SaveManager::setUsingStartpos(bool usingStartpos) {
// 	m_usingStartpos = usingStartpos;
// }

// GJGameLevel* SaveManager::m_level = nullptr;
// int SaveManager::m_levelCount = Mod::get()->getSavedValue<int>(LEVEL_COUNT_KEY, 0);
// Deaths SaveManager::m_deaths{};
// Deaths SaveManager::m_sessionDeaths{};
// Progresses SaveManager::m_progresses{};
// int SaveManager::m_checkpoint = 0;
// bool SaveManager::m_usingStartpos = false;
// bool SaveManager::m_shouldResetSessionDeaths = false;

// // DTPopupManager - manages popup state
// // ----------------------------
// CCSize DTPopupManager::getSize() {
// 	return SaveManager::hasNoDeaths(true)
// 		? CCSize(240.f, 160.f)
// 		: CCSize(160.f, WIN_SIZE.height * 0.85);
// }

// FLAlertLayer* DTPopupManager::getActiveInfoAlert() {
// 	return m_infoAlert;
// }

// bool DTPopupManager::isDTBtnEnabled() {
// 	return m_isDTBtnEnabled;
// }

// void DTPopupManager::enableDTBtn() {
// 	m_isDTBtnEnabled = true;
// }

// void DTPopupManager::toggleShowSessionDeaths() {
// 	m_showSessionDeaths ^= 1;
// }

// bool DTPopupManager::showSessionDeaths() {
// 	return m_showSessionDeaths;
// }

// void DTPopupManager::onInfoAlertOpen(FLAlertLayer* infoAlert, DTButtonLayer* dtBtn) {
// 	m_infoAlert = infoAlert;
// 	m_dtBtn = dtBtn;
// 	m_isDTBtnEnabled = true;
// 	handleTouchPriority(m_infoAlert);
// }

// void DTPopupManager::onInfoAlertClose(bool forceRemove) {
// 	m_isDTBtnEnabled = false;
// 	m_infoAlert = nullptr;
// 	DTPopupManager::removeDTBtn(forceRemove);
// 	SaveManager::setLevel(nullptr);
// }

// void DTPopupManager::onDTPopupOpen() {
// 	if (m_infoAlert == nullptr) return;
// 	m_infoAlert->setVisible(false);
// 	handleTouchPriority(m_infoAlert);
// }

// void DTPopupManager::onDTPopupClose() {
// 	m_showSessionDeaths = false;
// 	if (m_infoAlert == nullptr) return;
// 	m_infoAlert->setVisible(true);
// 	handleTouchPriority(m_infoAlert);
// }

// void DTPopupManager::removeDTBtn(bool forceRemove) {
// 	if (forceRemove && m_dtBtn != nullptr) m_dtBtn->removeFromParent();
// 	m_dtBtn = nullptr;
// }

// FLAlertLayer* DTPopupManager::m_infoAlert = nullptr;
// DTButtonLayer* DTPopupManager::m_dtBtn = nullptr;
// bool DTPopupManager::m_isDTBtnEnabled = false;
// bool DTPopupManager::m_showSessionDeaths = false;

// // track deaths
// // ----------------------------
// class $modify(GJBaseGameLayer) {
// 	TodoReturn setStartPosObject(StartPosObject* startpos) {
// 		GJBaseGameLayer::setStartPosObject(startpos);
// 		SaveManager::setUsingStartpos(startpos != nullptr);
// 	}
// };

// class $modify(PlayLayer) {
// 	bool init(GJGameLevel* level, bool p1, bool p2) {
// 		if (!PlayLayer::init(level, p1, p2)) return false;
// 		SaveManager::setShouldResetSessionDeaths();
// 		return true;
// 	}

// 	TodoReturn checkpointActivated(CheckpointGameObject* checkpt) {
// 		PlayLayer::checkpointActivated(checkpt);

// 		if (m_isPracticeMode) return;
// 		SaveManager::incCheckpoint();
// 	}

// 	TodoReturn resetLevelFromStart() {
// 		PlayLayer::resetLevelFromStart();
// 		SaveManager::resetCheckpoint();
// 	}

// 	TodoReturn onQuit() {
// 		PlayLayer::onQuit();
// 		SaveManager::resetCheckpoint();
// 	}
// };

// class $modify(PlayerObject) {
// 	TodoReturn playerDestroyed(bool p0) {
// 		PlayerObject::playerDestroyed(p0);

// 		// disable in editor
// 		auto playLayer = PlayLayer::get();
// 		if (playLayer == nullptr) return;
// 		auto level = playLayer->m_level;

// 		// disable in practice
// 		if (playLayer->m_isPracticeMode) return;

// 		// platformer + startpos = no worky, incorrect death tracking
// 		if (SaveManager::isPlatformer() && SaveManager::isUsingStartpos()) return;

// 		// disable tracking deaths on completed levels
// 		if (
// 			Mod::get()->getSettingValue<bool>("disable-tracking-completed-levels")
// 			&& level->m_newNormalPercent2.value() == 100
// 			&& level->m_levelType == GJLevelType::Saved
// 		)
// 			return;

// 		// save deaths
// 		if (SaveManager::getLevel() != level)
// 			SaveManager::setLevel(level);

// 		SaveManager::addDeath(playLayer->getCurrentPercentInt());
// 	}
// };

// // DTPopup layer
// // ----------------------------
// bool DTPopup::setup(CCSize popupSize) {
// 	// setup variables
// 	m_popupSize = popupSize;

// 	// set title
// 	auto session = DTPopupManager::showSessionDeaths();

// 	this->setTitle(
// 		session ? "Session Deaths" : "Deaths",
// 		"goldFont.fnt",
// 		session ? 0.65f : 0.9f
// 	);

// 	// fix some misc default menu customization
// 	auto titleNode = m_title;
// 	titleNode->setPositionY(titleNode->getPositionY() - 4);
// 	m_closeBtn->setVisible(false);
// 	this->setOpacity(150);

// 	this->setupPopupPages();
// 	return true;
// }

// void DTPopup::createNewPageLayer() {
// 	if (m_pageLayer != nullptr)
// 		this->removeChild(m_pageLayer);

// 	m_pageLayer = CCLayer::create();
// 	this->addChild(m_pageLayer);
// }

// void DTPopup::setupPopupPages() {
// 	// show popup if no deaths recorded for this level
// 	if (SaveManager::hasNoDeaths(true)) {
// 		this->showNoDeathsPage();
// 		return;
// 	}

// 	// create pages
// 	this->createPages();

// 	// add back button
// 	auto backBtnMenu = CCMenu::create();

// 	backBtnMenu->setPosition({
// 		24.f,
// 		WIN_SIZE.height - 23
// 	});

// 	auto backBtn = CCMenuItemSpriteExtra::create(
// 		CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
// 		this,
// 		menu_selector(DTPopup::onClose)
// 	);

// 	backBtnMenu->addChild(backBtn);
// 	this->addChild(backBtnMenu);

// 	// show current page
// 	this->showPage();
// }

// // this is probably the worst way ever to do this
// // but idk what the fuck im doing :skull:
// void DTPopup::showNoDeathsPage() {
// 	this->createNewPageLayer();

// 	// add labels
// 	auto labelsContainer = CCNode::create();
// 	labelsContainer->setAnchorPoint({0.5f, 0.5f});

// 	labelsContainer->setPosition({
// 		WIN_SIZE.width / 2,
// 		(WIN_SIZE.height / 2) + 7
// 	});

// 	labelsContainer->setContentSize({
// 		m_popupSize.width,
// 		m_popupSize.height
// 	});

// 	labelsContainer->setScale(0.45f);

// 	auto label1 = CCLabelBMFont::create("No deaths recorded.", "bigFont.fnt");
// 	auto label2 = CCLabelBMFont::create("Start playing the level", "bigFont.fnt");
// 	auto label3 = CCLabelBMFont::create("to record your deaths!", "bigFont.fnt");

// 	labelsContainer->addChild(label1);
// 	labelsContainer->addChild(label2);
// 	labelsContainer->addChild(label3);

// 	auto labelsLayout = ColumnLayout::create();
// 	labelsLayout->setAxisReverse(true);
// 	labelsContainer->setLayout(labelsLayout);

// 	this->addChild(labelsContainer);

// 	// add OK button
// 	auto okBtn = CCMenuItemSpriteExtra::create(
// 		ButtonSprite::create("OK"),
// 		this,
// 		menu_selector(DTPopup::onClose)
// 	);

// 	auto menu = CCMenu::create();
// 	auto btnSize = okBtn->getContentSize();

// 	menu->setPosition({
// 		WIN_SIZE.width / 2,
// 		(WIN_SIZE.height / 2) - (m_popupSize.height / 2) + (btnSize.height)
// 	});

// 	menu->addChild(okBtn);
// 	m_pageLayer->addChild(menu);
// 	handleTouchPriority(this);
// }

// void DTPopup::createPages() {
// 	auto deaths = SaveManager::getDeaths();

// 	int i = 0;
// 	int pageLen = SaveManager::isPlatformer() ? 10 : 13;
// 	int curPageLen = 0;
// 	int curPage = 0;

// 	for (auto& count : deaths) {
// 		int percent = i++;
// 		if (!count) continue;

// 		// add to curPage
// 		if (!curPageLen++) m_pages.push_back({});
// 		m_pages[curPage].push_back(percent);

// 		// go to next page
// 		if (curPageLen == pageLen) {
// 			curPageLen = 0;
// 			curPage++;
// 		}
// 	}
// }

// void DTPopup::nextPage(CCObject* sender) {
// 	if (m_pageIndex == m_pages.size() - 1) return;
// 	m_pageIndex++;
// 	showPage();
// }

// void DTPopup::backPage(CCObject* sender) {
// 	if (!m_pageIndex) return;
// 	m_pageIndex--;
// 	showPage();
// }

// void DTPopup::onCopy(CCObject* sender) {
// 	try {
// 		std::stringstream ss;

// 		auto deaths = SaveManager::getDeaths();
// 		int i = 0;

// 		for (auto& count : deaths) {
// 			int percent = i++;
// 			if (!count) continue;

// 			if (SaveManager::isPlatformer()) {
// 				ss << std::format("checkpoint {}: x{}", percent, count) << std::endl;
// 			} else {
// 				ss << std::format("{}%x{}", percent, count);

// 				if (SaveManager::isNewBest(percent))
// 					ss << " (new best)";

// 				ss << std::endl;
// 			}
// 		}

// 		clipboard::write(ss.str());

// 		Notification::create(
// 			std::string("Deaths copied successfully"),
// 			CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png")
// 		)->show();
// 	} catch (const std::exception& e) {
// 		Notification::create(
// 			std::string("Failed to copy deaths"),
// 			CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png")
// 		)->show();
// 	}
// }

// void DTPopup::onToggleSessionDeaths(CCObject* sender) {
// 	DTPopupManager::toggleShowSessionDeaths();

// 	// re-create popup
// 	auto parent = this->getParent();
// 	this->removeFromParent();
// 	parent->addChild(DTPopup::create());
// }

// void DTPopup::showPage() {
// 	this->createNewPageLayer();

// 	int heightTopOffset = 40;
// 	int padding = 19;

// 	// create copy button
// 	auto cpyBtnSpr = ButtonSprite::create("COPY");
// 	cpyBtnSpr->setScale(0.7f);

// 	auto cpyBtn = CCMenuItemSpriteExtra::create(
// 		cpyBtnSpr,
// 		this,
// 		menu_selector(DTPopup::onCopy)
// 	);

// 	auto cpyBtnSize = cpyBtn->getContentSize();

// 	// create session button
// 	auto sessionBtnSpr = CCSprite::create("dt_sessionBtn.png"_spr);
// 	sessionBtnSpr->setScale(0.525f);

// 	auto sessionBtn = CCMenuItemSpriteExtra::create(
// 		sessionBtnSpr,
// 		this,
// 		menu_selector(DTPopup::onToggleSessionDeaths)
// 	);

// 	auto btnsMenu = CCMenu::create();
// 	btnsMenu->addChild(cpyBtn);
// 	btnsMenu->addChild(sessionBtn);
// 	btnsMenu->setContentSize({m_popupSize.width, 0.f});

// 	btnsMenu->setPosition({
// 		WIN_SIZE.width / 2,
// 		(WIN_SIZE.height / 2) - (m_popupSize.height / 2) + (cpyBtnSize.height)
// 	});

// 	auto btnsMenuLayout = RowLayout::create();
// 	btnsMenuLayout->setGap(5.f);
// 	btnsMenu->setLayout(btnsMenuLayout);

// 	m_pageLayer->addChild(btnsMenu);

// 	// show next button
// 	if (m_pageIndex < m_pages.size() - 1) {
// 		auto nxtBtn = CCMenuItemSpriteExtra::create(
// 			CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
// 			this,
// 			menu_selector(DTPopup::nextPage)
// 		);

// 		auto nxtBtnMenu = CCMenu::create();

// 		nxtBtnMenu->setPosition({
// 			(WIN_SIZE.width / 2) + (m_popupSize.width / 2) + padding,
// 			WIN_SIZE.height / 2
// 		});

// 		nxtBtnMenu->setScaleX(-0.8f);
// 		nxtBtnMenu->setScaleY(0.8f);
// 		nxtBtnMenu->setAnchorPoint({0.f, 0.f});
// 		nxtBtnMenu->addChild(nxtBtn);
// 		m_pageLayer->addChild(nxtBtnMenu);
// 	}

// 	// show back button
// 	if (m_pageIndex > 0) {
// 		auto bckBtn = CCMenuItemSpriteExtra::create(
// 			CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
// 			this,
// 			menu_selector(DTPopup::backPage)
// 		);

// 		auto bckBtnMenu = CCMenu::create();

// 		bckBtnMenu->setPosition({
// 			(WIN_SIZE.width / 2) - (m_popupSize.width / 2) - padding,
// 			WIN_SIZE.height / 2
// 		});

// 		bckBtnMenu->setScale(0.8f);
// 		bckBtnMenu->setAnchorPoint({0.f, 0.f});
// 		bckBtnMenu->addChild(bckBtn);
// 		m_pageLayer->addChild(bckBtnMenu);
// 	}

// 	// show page deaths
// 	auto layout = ColumnLayout::create();
// 	layout->setAxisReverse(true);
// 	layout->setAxisAlignment(AxisAlignment::End);
// 	layout->setGap(SaveManager::isPlatformer() ? 6.f : 3.f);

// 	auto deathsNode = CCNode::create();

// 	deathsNode->setPosition({
// 		WIN_SIZE.width / 2,
// 		(WIN_SIZE.height / 2) + (m_popupSize.height / 2) - heightTopOffset
// 	});

// 	deathsNode->setContentSize({
// 		m_popupSize.width - (padding * 2),
// 		m_popupSize.height - heightTopOffset - cpyBtnSize.height - padding + 5
// 	});

// 	deathsNode->setAnchorPoint({0.5f, 1.f});
// 	deathsNode->setLayout(layout);

// 	auto deaths = SaveManager::getDeaths();

// 	for (auto& percent : m_pages[m_pageIndex]) {
// 		int count = deaths[percent];

// 		if (SaveManager::isPlatformer()) {
// 			auto checkpt = std::to_string(percent);
// 			auto countStr = std::format("x{}", count);

// 			auto checkptSpr = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
// 			checkptSpr->setScale(0.65f);

// 			auto checkptLbl = CCLabelBMFont::create(checkpt.c_str(), "bigFont.fnt");
// 			checkptLbl->setZOrder(2);
// 			checkptLbl->setScale(0.35f);

// 			auto checkptLblShadow = CCLabelBMFont::create(checkpt.c_str(), "bigFont.fnt");
// 			checkptLblShadow->setZOrder(1);
// 			checkptLblShadow->setScale(0.35f);

// 			auto checkptLblPos = checkptLbl->getPosition();
// 			checkptLblShadow->setPosition({
// 				checkptLblPos.x + 0.5f,
// 				checkptLblPos.y - 0.5f
// 			});

// 			checkptLblShadow->setColor({0, 0, 0});
// 			checkptLblShadow->setOpacity(120);

// 			auto countLbl = CCLabelBMFont::create(countStr.c_str(), "chatFont.fnt");
// 			countLbl->setAnchorPoint({0.f, 0.5f});

// 			auto checkptNode = CCNode::create();
// 			checkptNode->addChild(checkptSpr);
// 			checkptNode->addChild(checkptLbl);
// 			checkptNode->addChild(checkptLblShadow);
// 			checkptNode->setAnchorPoint({0.5f, 0.5f});

// 			checkptNode->setContentSize({
// 				(checkptLbl->getContentSize().width * checkptLbl->getScale()),
// 				0
// 			});

// 			auto node = CCNode::create();
// 			node->setAnchorPoint({0.5f, 0.5f});

// 			node->setContentSize({
// 				m_popupSize.width - (padding * 2),
// 				0
// 			});

// 			node->addChild(checkptNode);
// 			node->addChild(countLbl);
// 			deathsNode->addChild(node);

// 			auto layout = RowLayout::create();
// 			node->setLayout(layout);
// 			checkptNode->ignoreAnchorPointForPosition(true); // idk what this does, but it works! :grin:
// 		} else {
// 			auto labelStr = std::format("{}% x{}", percent, count);
// 			auto label = CCLabelBMFont::create(labelStr.c_str(), "chatFont.fnt");

// 			// new bests are yellow
// 			if (!DTPopupManager::showSessionDeaths() && SaveManager::isNewBest(percent))
// 				label->setColor({255, 255, 0});

// 			deathsNode->addChild(label);
// 		}
// 	}

// 	deathsNode->setLayout(layout);
// 	m_pageLayer->addChild(deathsNode);
// 	handleTouchPriority(this);
// }

// void DTPopup::keyDown(cocos2d::enumKeyCodes key) {
// 	switch (key) {
// 		case KEY_Left:
// 		case CONTROLLER_Left: {
// 			this->backPage(nullptr);
// 			break;
// 		}

// 		case KEY_Right:
// 		case CONTROLLER_Right: {
// 			this->nextPage(nullptr);
// 			break;
// 		}
// 	}

// 	Popup::keyDown(key);
// }

// void DTPopup::onClose(CCObject* sender) {
// 	Popup::onClose(sender);
// 	DTPopupManager::onDTPopupClose();
// }

// DTPopup* DTPopup::create() {
// 	auto ret = new DTPopup();
// 	auto popupSize = DTPopupManager::getSize();

// 	if (ret && ret->init(popupSize.width, popupSize.height, popupSize, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
// 		ret->autorelease();
// 		return ret;
// 	}

// 	CC_SAFE_DELETE(ret);
// 	return nullptr;
// }

// // DTButtonLayer layer
// // ----------------------------
// bool DTButtonLayer::init() {
// 	if (!CCLayer::init()) return false;

// 	auto btnSprite = CCSprite::create("dt_skullBtn_001.png"_spr);
// 	btnSprite->setScale(0.25f);

// 	auto btn = CCMenuItemSpriteExtra::create(
// 		btnSprite,
// 		this,
// 		menu_selector(DTButtonLayer::onOpenDTPopup)
// 	);

// 	auto btnSize = btn->getContentSize();

// 	btn->setPosition({
// 		-(btnSize.width - 2),
// 		btnSize.height - 2.5f
// 	});

// 	auto menu = CCMenu::create();
// 	menu->addChild(btn);
// 	this->addChild(menu);

// 	return true;
// }

// DTButtonLayer* DTButtonLayer::create() {
// 	auto ret = new DTButtonLayer();

// 	if (ret && ret->init()) {
// 		ret->autorelease();
// 		return ret;
// 	}

// 	CC_SAFE_DELETE(ret);
// 	return nullptr;
// }

// void DTButtonLayer::onOpenDTPopup(CCObject* sender) {
// 	// prevent crashes
// 	if (
// 		DTPopupManager::getActiveInfoAlert() == nullptr
// 		|| SaveManager::getLevel() == nullptr
// 	) return;

// 	DTPopupManager::onDTPopupOpen();

// 	auto scene = CCDirector::sharedDirector()->getRunningScene();
// 	scene->addChild(DTPopup::create());
// }

// // hooks to add DeathTrackerLayer
// // ----------------------------
// class $modify(LevelInfoLayer) {
// 	void onLevelInfo(CCObject* sender) {
// 		DTPopupManager::enableDTBtn();
// 		SaveManager::setLevel(m_level);
// 		LevelInfoLayer::onLevelInfo(sender);
// 	}
// };

// class $modify(EditLevelLayer) {
// 	void onLevelInfo(CCObject* sender) {
// 		DTPopupManager::enableDTBtn();
// 		SaveManager::setLevel(m_level);
// 		EditLevelLayer::onLevelInfo(sender);
// 	}
// };

// class $modify(LevelPage) {
// 	void onInfo(CCObject* sender) {
// 		// only show for actual main levels
// 		if (m_level->m_levelID.value() > 0) {
// 			DTPopupManager::enableDTBtn();
// 			SaveManager::setLevel(m_level);
// 		}

// 		LevelPage::onInfo(sender);
// 	}
// };

// class $modify(CCLayerColor) {
// 	bool initWithColor(const ccColor4B& color) {
// 		if (!CCLayerColor::initWithColor(color)) return false;

// 		// remove the m_dtBtn as a fail safe
// 		// this is guaranteed to fire before FLAlertLayer::init()
// 		DTPopupManager::removeDTBtn();
// 		return true;
// 	}
// };

// class $modify(DTAlertLayer, FLAlertLayer) {
// 	bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
// 		if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8))
// 			return false;

// 		if (DTPopupManager::isDTBtnEnabled()) {
// 			auto alertBgSize = getChildOfType<CCScale9Sprite>(this->m_mainLayer, 0)->getContentSize();

// 			auto dtBtn = DTButtonLayer::create();
// 			dtBtn->setZOrder(100);

// 			dtBtn->setPosition({
// 				alertBgSize.width / 2,
// 				-alertBgSize.height / 2
// 			});

// 			this->m_mainLayer->addChild(dtBtn);
// 			DTPopupManager::onInfoAlertOpen(this, dtBtn);
// 		}

// 		return true;
// 	}

// 	// manually hide the DT button
// 	// if cvolton.betterinfo is loaded
// 	void show() {
// 		FLAlertLayer::show();

// 		if (!DTPopupManager::isDTBtnEnabled()) return;
// 		if (!Loader::get()->isModLoaded("cvolton.betterinfo")) return;
// 		if (this->getChildByIDRecursive("cvolton.betterinfo/next-button") != nullptr) return;
// 		DTPopupManager::onInfoAlertClose(true);
// 	}

// 	void onBtn1(CCObject* sender) {
// 		if (this->m_button1->getParent() == sender)
// 			DTPopupManager::onInfoAlertClose();

// 		FLAlertLayer::onBtn1(sender);
// 	}

// 	void keyBackClicked() {
// 		DTPopupManager::onInfoAlertClose();
// 		FLAlertLayer::keyBackClicked();
// 	}
// };