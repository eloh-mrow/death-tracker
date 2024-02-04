#include <map>
#include <format>

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;
typedef std::vector<int> Deaths;
typedef std::map<int, bool> Progresses;

// globals
// ----------------------------
auto WIN_SIZE = CCDirector::sharedDirector()->getWinSize();
int DT_POPUP_PAGE_LEN = 13;

// SaveManager - helps with save data
// ----------------------------
class SaveManager {
private:
	static GJGameLevel* m_level;
	static int m_levelCount;
	static Deaths m_deaths;
	static Progresses m_progresses;

	// TODO: add backups
	static void createBackup() {}

	static std::string getLevelId() {
		if (m_level == nullptr) return "";

		auto levelId = std::to_string(m_level->m_levelID.value());

		// local level postfix
		if (m_level->m_levelType != GJLevelType::Saved)
			levelId += "-local";

		// daily/weekly postfix
		if (m_level->m_dailyID > 0)
			levelId += "-daily";

		// gauntlet level postfix
		if (m_level->m_gauntletLevel)
			levelId += "-gauntlet";

		return levelId;
	}

	static void calcDeathsAndProgresses() {
		if (m_level == nullptr) return;

		// calculate progresses
		// derived from cvolton.betterinfo
		std::string personalBests = m_level->m_personalBests;
		int percentage = m_level->m_newNormalPercent2;

		Progresses progresses{};
		std::stringstream bestsStream(personalBests);
		std::string currentBest;
		int currentPercent = 0;

		while (getline(bestsStream, currentBest, ',')) {
			currentPercent += std::stoi(currentBest);
			progresses[currentPercent] = true;
		}

		// calculate deaths
		auto levelId = getLevelId();
		auto deaths = Mod::get()->getSavedValue<Deaths>(levelId);

		// default deaths to progresses x1
		auto pbs = std::string(m_level->m_personalBests);

		if (deaths.empty() && pbs != "" && pbs != "100") {
			deaths = Deaths(100);

			for (auto& [percent, _] : progresses) {
				if (percent == 100) continue; // can't die at 100%
				deaths[percent] = 1;
			}
		}

		m_progresses = progresses;
		m_deaths = deaths;
	}

public:
	SaveManager() = delete;

	static GJGameLevel* getLevel() {
		return m_level;
	}

	static void setLevel(GJGameLevel* level) {
		m_level = level;

		if (level != nullptr) {
			SaveManager::calcDeathsAndProgresses();
		} else {
			m_progresses = Progresses();
			m_deaths = Deaths();
		}
	}

	static bool isNewBest(int percent) {
		return m_progresses[percent];
	}

	static int getLevelCount() {
		return Mod::get()->getSavedValue<int>("levelCount", 0);
	}

	static void setLevelCount(int levelCount) {
		Mod::get()->setSavedValue("levelCount", levelCount);
	}

	static Deaths getDeaths() {
		return m_deaths;
	}

	static void addDeath(int percent) {
		// new unplayed level
		if (m_deaths.empty()) {
			SaveManager::setLevelCount(++m_levelCount);
			m_deaths = Deaths(100);

			// create new backup
			// every 50 levels
			if (m_levelCount % 50 == 0)
				SaveManager::createBackup();
		}

		m_deaths[percent]++;

		auto levelId = getLevelId();
		Mod::get()->setSavedValue(levelId, m_deaths);
	}
};

GJGameLevel* SaveManager::m_level = nullptr;
int SaveManager::m_levelCount = SaveManager::getLevelCount();
Deaths SaveManager::m_deaths{};
Progresses SaveManager::m_progresses{};

// DTPopupManager - manages popup state
// ----------------------------
class DTPopupManager {
private:
	static FLAlertLayer* m_infoAlert;
	static bool m_isDTBtnEnabled;

public:
	DTPopupManager() = delete;

	static FLAlertLayer* getActiveInfoAlert() {
		return m_infoAlert;
	}

	static bool isDTBtnEnabled() {
		return m_isDTBtnEnabled;
	}

	static void enableDTBtn() {
		m_isDTBtnEnabled = true;
	}

	static void onInfoAlertOpen(FLAlertLayer* infoAlert) {
		m_infoAlert = infoAlert;
		m_isDTBtnEnabled = true;
		handleTouchPriority(m_infoAlert);
	}

	static void onInfoAlertClose() {
		m_isDTBtnEnabled = false;
		m_infoAlert = nullptr;
		SaveManager::setLevel(nullptr);
	}

	static void onDTPopupOpen() {
		if (m_infoAlert == nullptr) return;
		m_infoAlert->setVisible(false);
		handleTouchPriority(m_infoAlert);
	}

	static void onDTPopupClose() {
		if (m_infoAlert == nullptr) return;
		m_infoAlert->setVisible(true);
		handleTouchPriority(m_infoAlert);
	}
};

FLAlertLayer* DTPopupManager::m_infoAlert = nullptr;
bool DTPopupManager::m_isDTBtnEnabled = false;

// track deaths
// ----------------------------
class $modify(PlayerObject) {
	TodoReturn playerDestroyed(bool p0) {
		PlayerObject::playerDestroyed(p0);

		auto playLayer = PlayLayer::get();
		if (playLayer == nullptr) return; // disable in editor

		auto level = playLayer->m_level;
		if (playLayer->m_isPracticeMode) return; // disable in practice
		if (level->isPlatformer()) return; // disable for platformer

		// disable tracking deaths on completed levels
		if (
			Mod::get()->getSettingValue<bool>("disable-tracking-completed-levels")
			&& level->m_newNormalPercent2.value() == 100
			&& level->m_levelType == GJLevelType::Saved
		)
			return;

		// save deaths
		if (SaveManager::getLevel() != level)
			SaveManager::setLevel(level);

		SaveManager::addDeath(playLayer->getCurrentPercentInt());
	}
};

// custom layers
// ----------------------------
class DTPopup : public Popup<CCSize> {
protected:
	CCSize m_popupSize;
	CCNode* m_pageLayer = nullptr;
	int m_pageIndex = 0;
	std::vector<std::vector<int>> m_pages;

	bool setup(CCSize popupSize) override {
		// setup variables
		m_popupSize = popupSize;

		// set title
		this->setTitle("Deaths", "goldFont.fnt", 0.9f);

		// fix some misc default menu customization
		auto titleNode = m_title;
		titleNode->setPositionY(titleNode->getPositionY() - 4);
		m_closeBtn->setVisible(false);
		this->setOpacity(150);

		// get deaths
		auto deaths = SaveManager::getDeaths();

		// show popup if no deaths recorded for this level
		// this is probably the worst way ever to do this
		// but idk what the fuck im doing :skull:
		if (deaths.empty()) {
			// add labels
			auto labelsContainer = CCNode::create();
			labelsContainer->setAnchorPoint({0.5f, 0.5f});

			labelsContainer->setPosition({
				WIN_SIZE.width / 2,
				(WIN_SIZE.height / 2) + 7
			});

			labelsContainer->setContentSize({
				popupSize.width,
				popupSize.height
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

			this->addChild(labelsContainer);

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
				(WIN_SIZE.height / 2) - (popupSize.height / 2) + (btnSize.height)
			});

			menu->addChild(okBtn);
			this->addChild(menu);

			return true;
		}

		// create pages
		int i = 0;
		int curPageLen = 0;
		int curPage = 0;

		for (auto& count : deaths) {
			int percent = i++;
			if (!count) continue;

			// add to curPage
			if (!curPageLen++) m_pages.push_back({});
			m_pages[curPage].push_back(percent);

			// go to next page
			if (curPageLen == DT_POPUP_PAGE_LEN) {
				curPageLen = 0;
				curPage++;
			}
		}

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

		return true;
	}

	void nextPage(CCObject* sender) {
		if (m_pageIndex == m_pages.size() - 1) return;
		m_pageIndex++;
		showPage();
	}

	void backPage(CCObject* sender) {
		if (!m_pageIndex) return;
		m_pageIndex--;
		showPage();
	}

	void onCopy(CCObject* sender) {
		try {
			std::stringstream ss;

			auto deaths = SaveManager::getDeaths();
			int i = 0;

			for (auto& count : deaths) {
				int percent = i++;
				if (!count) continue;

				ss << std::format("{}%x{}", percent, count);

				if (SaveManager::isNewBest(percent))
					ss << " (new best)";

				ss << std::endl;
			}

			clipboard::write(ss.str());

			Notification::create(
				std::string("Deaths copied successfully"),
				CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png")
			)->show();
		} catch (const std::exception& e) {
			Notification::create(
				std::string("Failed to copy deaths"),
				CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png")
			)->show();
		}
	}

	void showPage() {
		// remove old page
		if (m_pageLayer != nullptr)
			this->removeChild(m_pageLayer);

		auto pageLayer = CCLayer::create();
		int heightTopOffset = 40;
		int padding = 19;

		// show copy button
		auto cpyBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("COPY"),
			this,
			menu_selector(DTPopup::onCopy)
		);

		auto cpyBtnSize = cpyBtn->getContentSize();
		auto cpyBtnMenu = CCMenu::create();

		cpyBtnMenu->setPosition({
			WIN_SIZE.width / 2,
			(WIN_SIZE.height / 2) - (m_popupSize.height / 2) + (cpyBtnSize.height) - 5
		});

		cpyBtnMenu->setAnchorPoint({0.f, 0.f});
		cpyBtnMenu->setScale(0.7f);
		cpyBtnMenu->addChild(cpyBtn);
		pageLayer->addChild(cpyBtnMenu);

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
			pageLayer->addChild(nxtBtnMenu);
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
			pageLayer->addChild(bckBtnMenu);
		}

		// show page deaths
		auto layout = ColumnLayout::create();
		layout->setAxisReverse(true);
		layout->setAxisAlignment(AxisAlignment::End);
		layout->setGap(3.f);

		auto deathsNode = CCNode::create();

		deathsNode->setPosition({
			WIN_SIZE.width / 2,
			(WIN_SIZE.height / 2) + (m_popupSize.height / 2) - heightTopOffset
		});

		deathsNode->setContentSize({
			m_popupSize.width - (padding * 2),
			m_popupSize.height - heightTopOffset - cpyBtnSize.height - padding + 5 + 4
		});

		deathsNode->setAnchorPoint({0.5f, 1.f});
		deathsNode->setLayout(layout);

		auto deaths = SaveManager::getDeaths();

		for (auto& percent : m_pages[m_pageIndex]) {
			int count = deaths[percent];
			auto labelStr = std::format("{}% x{}", percent, count);
			auto label = CCLabelBMFont::create(labelStr.c_str(), "chatFont.fnt");

			// new bests are yellow
			if (SaveManager::isNewBest(percent))
				label->setColor({255, 255, 0});

			deathsNode->addChild(label);
		}

		deathsNode->setLayout(layout);
		pageLayer->addChild(deathsNode);

		// show page layer
		m_pageLayer = pageLayer;
		this->addChild(pageLayer);
		handleTouchPriority(this);
	}

	void keyDown(cocos2d::enumKeyCodes key) {
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

	virtual void onClose(CCObject* sender) {
		Popup::onClose(sender);
		DTPopupManager::onDTPopupClose();
	}

public:
	static DTPopup* create(CCSize popupSize) {
		auto ret = new DTPopup();

		if (ret && ret->init(popupSize.width, popupSize.height, popupSize, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
			ret->autorelease();
			return ret;
		}

		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

class DTButtonLayer : public CCLayer {
protected:
	bool init() {
		if (!CCLayer::init()) return false;

		auto btnSprite = CCSprite::create("dt_skullBtn_001-uhd.png"_spr);
		btnSprite->setScale(0.9f);

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

public:
	static DTButtonLayer* create() {
		auto ret = new DTButtonLayer();

		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}

		CC_SAFE_DELETE(ret);
		return nullptr;
	}

	void onOpenDTPopup(CCObject* sender) {
		// prevent crashes
		if (
			DTPopupManager::getActiveInfoAlert() == nullptr
			|| SaveManager::getLevel() == nullptr
		) return;

		DTPopupManager::onDTPopupOpen();
		auto deaths = SaveManager::getDeaths();

		auto popupSize = deaths.empty()
			? CCSize(240.f, 160.f)
			: CCSize(160.f, WIN_SIZE.height * 0.85);

		auto scene = CCDirector::sharedDirector()->getRunningScene();
		scene->addChild(DTPopup::create(popupSize));
	}
};

// hooks to add DeathTrackerLayer
// ----------------------------
class $modify(LevelInfoLayer) {
	void onLevelInfo(CCObject* sender) {
		if (!m_level->isPlatformer()) {
			DTPopupManager::enableDTBtn();
			SaveManager::setLevel(m_level);
		}

		LevelInfoLayer::onLevelInfo(sender);
	}
};

class $modify(EditLevelLayer) {
	void onLevelInfo(CCObject* sender) {
		if (!m_level->isPlatformer()) {
			DTPopupManager::enableDTBtn();
			SaveManager::setLevel(m_level);
		}

		EditLevelLayer::onLevelInfo(sender);
	}
};

class $modify(LevelPage) {
	void onInfo(CCObject* sender) {
		// only show for actual main levels
		if (m_level->m_levelID.value() > 0) {
			DTPopupManager::enableDTBtn();
			SaveManager::setLevel(m_level);
		}

		LevelPage::onInfo(sender);
	}
};

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
			DTPopupManager::onInfoAlertOpen(this);
		}

		return true;
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