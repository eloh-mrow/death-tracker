#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class SavePathSettingValue : public SettingValue {
protected:
    std::string m_SavePath;

public:
    SavePathSettingValue(std::string const& key, std::string const& modID, std::string savePath)
        : SettingValue(key, modID), m_SavePath(savePath) {}

    bool load(matjson::Value const& json) override {
        if (!json.is<std::string>()) return false;
        m_SavePath = json.as<std::string>();
        return true;
    }

    bool save(matjson::Value& json) const override {
        json = m_SavePath;
        return true;
    }

    SettingNode* createNode(float width) override;

    void setPath(std::string savePath) {
        m_SavePath = savePath;
    }

    std::string getSavedPath() const {
        return m_SavePath;
    }
};