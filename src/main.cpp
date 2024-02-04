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

bool showDTButtonLayer = false;
CCNode* infoAlert = nullptr;
GJGameLevel* level = nullptr;
auto winSize = CCDirector::sharedDirector()->getWinSize();

void resetDTPopup() {
	showDTButtonLayer = false;
	infoAlert = nullptr;
	level = nullptr;
}

// derived from cvolton.betterinfo
std::vector<int> calcProgresses(std::string personalBests, int percentage) {
    std::stringstream bestsStream(personalBests);
    std::string currentBest;
    std::deque<int> progressDeltas;

    while (getline(bestsStream, currentBest, ',')) {
        progressDeltas.push_front(std::stoi(currentBest));
    }

	std::vector<int> progresses;

	for (auto delta : progressDeltas) {
		progresses.push_back(percentage);
		percentage -= delta;
	}

	return progresses;
}

// save data helper functions
// ----------------------------
std::string getLevelId(GJGameLevel* level) {
	auto levelId = std::to_string(level->m_levelID.value());

	// local level postfix
	if (level->m_levelType != GJLevelType::Saved)
		levelId += "-local";

	// daily/weekly postfix
	if (level->m_dailyID > 0)
		levelId += std::to_string(level->m_dailyID.value());

	// gauntlet level postfix
	if (level->m_gauntletLevel)
		levelId += "-gauntlet";

	return levelId;
}

int getLevelCount() {
	return Mod::get()->getSavedValue<int>("levelCount", 0);
}

void setLevelCount(int levelCount) {
	Mod::get()->setSavedValue("levelCount", levelCount);
}

Deaths getDeaths(GJGameLevel* level) {
	auto levelId = getLevelId(level);
	auto deaths = Mod::get()->getSavedValue<Deaths>(levelId);

	// default deaths to progresses x1
	auto pbs = std::string(level->m_personalBests);

	if (deaths.empty() && pbs != "" && pbs != "100") {
		deaths = Deaths(100);
		auto progresses = calcProgresses(level->m_personalBests, level->m_newNormalPercent2);

		for (auto& percent : progresses) {
			if (percent == 100) continue; // can't die at 100%
			deaths[percent] = 1;
		}
	}

	return deaths;
}

void setDeaths(GJGameLevel* level, Deaths deaths) {
	auto levelId = getLevelId(level);
	Mod::get()->setSavedValue(levelId, deaths);
}

// track deaths
// ----------------------------
class $modify(PlayerObject) {
	TodoReturn playerDestroyed(bool p0) {
		PlayerObject::playerDestroyed(p0);

		auto playLayer = PlayLayer::get();
		if (!playLayer) return; // disable in editor
		if (playLayer->m_isPracticeMode) return; // disable in practice
		if (playLayer->m_level->isPlatformer()) return; // disable for platformer

		// save deaths
		auto level = playLayer->m_level;
		auto percent = playLayer->getCurrentPercentInt();

		auto deaths = getDeaths(level);
		int levelCount = getLevelCount();

		// fill with 0s
		// increment levelCount
		if (deaths.empty()) {
			deaths = Deaths(100);
			setLevelCount(++levelCount);
		}

		// TODO: add backups
		if (levelCount % 50 == 0) {
		}

		deaths[percent]++;
		setDeaths(level, deaths);
	}
};

// custom layers
// ----------------------------
class DTPopup : public Popup<CCSize> {
protected:
	CCSize m_popupSize;
	Deaths m_deaths;
	CCNode* m_pageLayer = nullptr;
	int m_pageLength = 15;
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
		m_deaths = getDeaths(level);

		// show popup if no deaths recorded for this level
		// this is probably the worst way ever to do this
		// but idk what the fuck im doing :skull:
		if (m_deaths.empty()) {
			// add labels
			auto labelsContainer = CCNode::create();
			labelsContainer->setAnchorPoint(CCPoint(0.5f, 0.5f));

			labelsContainer->setPosition(CCSize(
				winSize.width / 2,
				(winSize.height / 2) + 7
			));

			labelsContainer->setContentSize(CCSize(
				popupSize.width,
				popupSize.height
			));

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
			menu->setPositionY((winSize.height / 2) - (popupSize.height / 2) + (btnSize.height));
			menu->setPositionX(winSize.width / 2);
			menu->addChild(okBtn);
			this->addChild(menu);

			return true;
		}

		// create pages
		int i = 0;
		int curPageLen = 0;
		int curPage = 0;

		for (auto& count : m_deaths) {
			int percent = i++;
			if (!count) continue;

			// add to curPage
			if (!curPageLen++) m_pages.push_back({});
			m_pages[curPage].push_back(percent);

			// go to next page
			if (curPageLen == m_pageLength) {
				curPageLen = 0;
				curPage++;
			}
		}

		// add back button
		auto backBtnMenu = CCMenu::create();
		backBtnMenu->setPositionX(24.f);
		backBtnMenu->setPositionY(winSize.height - 23);

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

	void showPage() {
		// remove old page
		if (m_pageLayer != nullptr)
			this->removeChild(m_pageLayer);

		auto pageLayer = CCLayer::create();
		int heightTopOffset = 44;
		int padding = 19;

		// show next button
		if (m_pageIndex < m_pages.size() - 1) {
			auto nxtBtn = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
				this,
				menu_selector(DTPopup::nextPage)
			);

			auto nxtBtnMenu = CCMenu::create();

			nxtBtnMenu->setPosition(CCSize(
				(winSize.width / 2) + (m_popupSize.width / 2) + padding,
				winSize.height / 2
			));

			nxtBtnMenu->setScaleX(-0.8f);
			nxtBtnMenu->setScaleY(0.8f);
			nxtBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
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

			bckBtnMenu->setPosition(CCSize(
				(winSize.width / 2) - (m_popupSize.width / 2) - padding,
				winSize.height / 2
			));

			bckBtnMenu->setScale(0.8f);
			bckBtnMenu->setAnchorPoint(CCPoint(0.f, 0.f));
			bckBtnMenu->addChild(bckBtn);
			pageLayer->addChild(bckBtnMenu);
		}

		// show page deaths
		auto layout = ColumnLayout::create();
		layout->setAxisReverse(true);
		layout->setAxisAlignment(AxisAlignment::End);
		layout->setGap(3.f);

		auto deathsNode = CCNode::create();
		deathsNode->setPositionX((winSize.width / 2));
		deathsNode->setPositionY((winSize.height / 2) + (m_popupSize.height / 2) - heightTopOffset);

		deathsNode->setContentSize(CCSize(
			m_popupSize.width - (padding * 2), // width
			m_popupSize.height - (heightTopOffset + padding) // height
		));

		deathsNode->setAnchorPoint(CCPoint(0.5f, 1.f));
		deathsNode->setLayout(layout);

		for (auto& percent : m_pages[m_pageIndex]) {
			int count = m_deaths[percent];
			auto labelContent = std::format("{}% x{}", percent, count);
			auto label = CCLabelBMFont::create(labelContent.c_str(), "chatFont.fnt");
			label->setScale(0.6f);
			deathsNode->addChild(label);
		}

		deathsNode->setLayout(layout);
		pageLayer->addChild(deathsNode);

		// show page layer
		m_pageLayer = pageLayer;
		this->addChild(pageLayer);
		handleTouchPriority(this);
	}

	virtual void onClose(CCObject* sender) {
		Popup::onClose(sender);
		infoAlert->setVisible(true);
		handleTouchPriority(infoAlert);
	}

public:
	static DTPopup* create(CCSize popupSize) {
		auto ret = new DTPopup();

		if (ret && ret->init(popupSize.width, popupSize.height, popupSize, "square01_001.png", CCRect(0.f, 0.f, 94.f, 94.f))) {
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
		btnSprite->setScale(1.35f);

		auto btn = CCMenuItemSpriteExtra::create(
			btnSprite,
			this,
			menu_selector(DTButtonLayer::onOpenDTPopup)
		);

		auto menu = CCMenu::create();
		auto btnSize = btn->getContentSize();
		menu->addChild(btn);
		menu->setPositionY(btnSize.height / 2);

		auto btnSpritePos = btnSprite->getPosition();
		btnSprite->setPositionX(btnSpritePos.x - 8);
		btnSprite->setPositionY(btnSpritePos.y + 6);

		auto layout = RowLayout::create();
		layout->setAxisAlignment(AxisAlignment::End);
		menu->setLayout(layout);
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
		// just in case
		if (infoAlert == nullptr || level == nullptr)
			return;

		infoAlert->setVisible(false);

		auto deaths = getDeaths(level);

		auto popupSize = deaths.empty()
			? CCSize(240.f, 160.f)
			: CCSize(160.f, winSize.height * 0.85);

		auto scene = CCDirector::sharedDirector()->getRunningScene();
		scene->addChild(DTPopup::create(popupSize));
	}
};

// hooks to add DeathTrackerLayer
// ----------------------------
class $modify(LevelInfoLayer) {
	void onLevelInfo(CCObject* sender) {
		if (!m_level->isPlatformer()) {
			showDTButtonLayer = true;
			level = m_level;
		}

		LevelInfoLayer::onLevelInfo(sender);
	}
};

class $modify(EditLevelLayer) {
	void onLevelInfo(CCObject* sender) {
		if (!m_level->isPlatformer()) {
			showDTButtonLayer = true;
			level = m_level;
		}

		EditLevelLayer::onLevelInfo(sender);
	}
};

class $modify(LevelPage) {
	void onInfo(CCObject* sender) {
		// only show for actual main levels
		if (m_level->m_levelID.value() > 0) {
			showDTButtonLayer = true;
			level = m_level;
		}

		LevelPage::onInfo(sender);
	}
};

class $modify(DTAlertLayer, FLAlertLayer) {
	bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
		if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8))
			return false;

		if (showDTButtonLayer) {
			this->addChild(DTButtonLayer::create());
			infoAlert = this;
			handleTouchPriority(this);
		}

		return true;
	}

	void onBtn1(CCObject* sender) {
		resetDTPopup();
		FLAlertLayer::onBtn1(sender);
	}
};