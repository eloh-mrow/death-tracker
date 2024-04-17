#pragma once

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;

typedef struct {
    std::string labelName;
    std::string text;
    int line;
    int position;
    ccColor4B color;
    CCTextAlignment alignment;
    int font;
    float fontSize;
} LabelLayout;

template <>
struct matjson::Serialize<LabelLayout> {
    static LabelLayout from_json(const matjson::Value& value) {
        return LabelLayout {
            .labelName = value["labelName"].as_string(),
            .text = value["text"].as_string(),
            .line = value["line"].as_int(),
            .position = value["position"].as_int(),
            .color = value["color"].as<ccColor4B>(),
            .alignment = static_cast<CCTextAlignment>(value["alignment"].as_int()),
            .font = value["font"].as_int(),
            .fontSize = static_cast<float>(value["fontSize"].as_double())
        };
    }

    static matjson::Value to_json(const LabelLayout& value) {
        auto obj = matjson::Object();
        obj["labelName"] = value.labelName;
        obj["text"] = value.text;
        obj["line"] = value.line;
        obj["position"] = value.position;
        obj["color"] = value.color;
        obj["alignment"] = static_cast<int>(value.alignment);
        obj["font"] = value.font;
        obj["fontSize"] = value.fontSize;
        return obj;
    }
};

class Save {
    public:
        static std::vector<LabelLayout> getLayout();
        static void setLayout(std::vector<LabelLayout> layout);

        static ccColor3B getNewBestColor();
        static void setNewBestColor(ccColor3B color);

        static ccColor3B getSessionBestColor();
        static void setSessionBestColor(ccColor3B color);
};