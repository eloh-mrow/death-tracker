#include "Settings.hpp"
#include "../utils/SavePathSettingValue.hpp"

int64_t Settings::getMaxSessionLength() {
    return Mod::get()->getSettingValue<int64_t>("session-length");
}

bool Settings::isCompletedLevelTrackingDisabled() {
    return Mod::get()->getSettingValue<bool>("disable-tracking-completed-levels");
}

bool Settings::isPracticeZeroDeathsEnabled() {
    return Mod::get()->getSettingValue<bool>("practice-zero-deaths");
}

bool Settings::getPauseMenuEnabled(){
    return Mod::get()->getSettingValue<bool>("pause-menu");
}

bool Settings::getLateSaveEnabled(){
    return Mod::get()->getSettingValue<bool>("late-save");
}

std::filesystem::path Settings::getSavePath(){
    return std::filesystem::path{static_cast<SavePathSettingValue*>(Mod::get()->getSetting("save-path"))->getSavedPath()};
}

bool Settings::getSafeModeEnabled(){
    return Mod::get()->getSettingValue<bool>("safe-mode");
}