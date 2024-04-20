#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include "utils/Save.hpp"

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
                .text = "Session:{nl}{s0}{nl} ",
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
}