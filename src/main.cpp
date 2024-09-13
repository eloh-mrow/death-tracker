#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include "utils/Save.hpp"
#include "managers/StatsManager.hpp"

$execute {
    auto _ = file::createDirectory(Mod::get()->getSaveDir() / "levels");

    if (Save::getLayout().size() == 0){
        
        std::vector<LabelLayout> defaultLayout{
            {
                .labelName = "from 0",
                .text = "From 0:{nl}{f0}{nl} ",
                .line = 2,
                .position = 0,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Session",
                .text = "Session:{nl}{ssd}{nl}{s0}{nl} ",
                .line = 2,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Runs",
                .text = "Runs:{nl}{runs}{nl} ",
                .line = 3,
                .position = 0,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "SRuns",
                .text = "Session Runs:{nl}{sruns}{nl} ",
                .line = 3,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 0,
                .fontSize = 0.5f
            },
            {
                .labelName = "Title",
                .text = "{lvln}:",
                .line = 0,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 2,
                .fontSize = 1
            },
            {
                .labelName = "att",
                .text = "{att} attempts",
                .line = 1,
                .position = 1,
                .color = {255,255,255,255},
                .alignment = CCTextAlignment::kCCTextAlignmentCenter,
                .font = 1,
                .fontSize = 0.75f
            }
        };

        Save::setLayout(defaultLayout);

        Save::setNewBestColor({255, 255, 0});
        Save::setSessionBestColor({ 255, 136, 0 });
    }

    auto verNums = StatsManager::splitStr(Save::getLastOpenedVersion(), ".");

    if (verNums.size() < 2) return;

    VersionInfo* oldver = new VersionInfo(2, 1, 2);

    VersionInfo* oldsavedver = new VersionInfo(std::stoi(verNums[0]), std::stoi(verNums[1]), std::stoi(verNums[2]));

    log::info("{} | {}", oldver->toString(), oldsavedver->toString());

    if (oldver >= oldsavedver){
        if (Mod::get()->getSettingValue<int64_t>("session-length") == -2){
            Mod::get()->setSettingValue<std::string>("session-method", "Exit level");
            Mod::get()->setSettingValue<int64_t>("session-length", 1);
        }
        else if (Mod::get()->getSettingValue<int64_t>("session-length") == -1){
            Mod::get()->setSettingValue<std::string>("session-method", "Exit game");
            Mod::get()->setSettingValue<int64_t>("session-length", 1);
        }
        else if (Mod::get()->getSettingValue<int64_t>("session-length") >= 0){
            Mod::get()->setSettingValue<std::string>("session-method", "Seconds");
            log::info("----------------\n-----------------\n-------------------");

            if (Mod::get()->getSettingValue<int64_t>("session-length") == 0)
                Mod::get()->setSettingValue<int64_t>("session-length", 1);
        }
    }

    delete oldver;
    delete oldsavedver;
};
