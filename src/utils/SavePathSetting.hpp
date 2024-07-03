#pragma once

#include <Geode/Geode.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/UI/TextArea.hpp>

#include "SavePathSettingValue.hpp"
#include "utils/Settings.hpp"
#include "../managers/StatsManager.hpp"

using namespace geode::prelude;

std::filesystem::path defaultPath = Mod::get()->getSaveDir() / "levels";

class SavePathSetting : public SettingNode {
protected:
    std::string currentPath;
    SavePathSettingValue* myValue;
    CCMenuItemSpriteExtra* resetButton;
    InputNode* pathText;

    bool init(SavePathSettingValue* value, float width) {
        if (!SettingNode::init(value))
            return false;

        currentPath = value->getSavedPath();
        myValue = value;

        this->setContentSize({ width, 40.f });

        auto myMenu = CCMenu::create();
        myMenu->setPosition({0,0});
        this->addChild(myMenu);

        auto selectPathButtonS = ButtonSprite::create("Select Path");
        selectPathButtonS->setScale(0.6f);
        auto selectPathButton = CCMenuItemSpriteExtra::create(
            selectPathButtonS,
            nullptr,
            this,
            menu_selector(SavePathSetting::onSelectPath)
        );
        selectPathButton->setPosition({width - selectPathButton->getScaledContentSize().width / 2 - 5, 20});
        myMenu->addChild(selectPathButton);

        auto textBG = CCScale9Sprite::create("square02_001.png", {0,0, 80, 80});
        textBG->setContentSize(ccp(width / 1.7f, 35) * 2);
        textBG->setScale(0.5f);
        textBG->setOpacity(120);
        textBG->setPosition({textBG->getScaledContentSize().width / 2 + 5, 20});
        this->addChild(textBG);

        auto resetButtonS = CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png");
        resetButtonS->setScale(0.5f);
        resetButton = CCMenuItemSpriteExtra::create(
            resetButtonS,
            nullptr,
            this,
            menu_selector(SavePathSetting::resetMe)
        );
        resetButton->setVisible(false);
        resetButton->setPosition({width - resetButton->getScaledContentSize().width - selectPathButtonS->getScaledContentSize().width, 20});
        myMenu->addChild(resetButton);

        pathText = InputNode::create(textBG->getContentSize().width, "");
        pathText->setZOrder(1);
        pathText->setScale(0.5f);
        pathText->getBG()->setVisible(false);
        pathText->setString(currentPath);
        pathText->setPosition(textBG->getPosition());
        pathText->setEnabled(false);
        this->addChild(pathText);
        
        return true;
    }

    void onSelectPath(CCObject*){
        file::FilePickOptions options;
        options.defaultPath = myValue->getSavedPath();
        file::FilePickOptions::Filter filterlol;
        filterlol.description = "select file.";
        std::vector<file::FilePickOptions::Filter> filters = {filterlol};
        options.filters = filters;

        file::pick(file::PickMode::OpenFolder, options).listen([this](Result<std::filesystem::path>* path) {
                if (path->isOk()) {
                    currentPath = path->unwrap().string();
                    this->dispatchChanged();

                    if (hasUncommittedChanges()){
                        onChanged(true);
                    }
                    else{
                        onChanged(false);
                    }
                }
            },
            [] (auto progress) {},
            [] () {}
        );
    }

    void onChanged(bool changeIsNew){
        resetButton->setVisible(changeIsNew);
        pathText->setString(currentPath);
    }

    void resetMe(CCObject*){
        resetToDefault();
    }

public:
    // to save the setting
    void commit() override {
        myValue->setPath(currentPath);
        StatsManager::setPath(std::filesystem::path{currentPath});
        onChanged(false);
        this->dispatchCommitted();
    }

    bool hasUncommittedChanges() override {
        return currentPath != myValue->getSavedPath();
    }

    bool hasNonDefaultValue() override {
        return currentPath != defaultPath.string();
    }

    void resetToDefault() override {
        onChanged(false);
        currentPath = defaultPath.string();
    }

    static SavePathSetting* create(SavePathSettingValue* value, float width) {
        auto ret = new SavePathSetting();
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNode* SavePathSettingValue::createNode(float width) {
    return SavePathSetting::create(this, width);
}

$on_mod(Loaded) {
	Mod::get()->addCustomSetting<SavePathSettingValue>("save-path", defaultPath.string());
    StatsManager::setPath(Settings::getSavePath());
};
