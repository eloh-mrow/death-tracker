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
    static Result<LabelLayout> fromJson(const matjson::Value& value) {
        LabelLayout layout;
        GEODE_UNWRAP_INTO(layout.labelName, value["labelName"].asString());
        GEODE_UNWRAP_INTO(layout.text, value["text"].asString());
        GEODE_UNWRAP_INTO(layout.line, value["line"].asInt());
        GEODE_UNWRAP_INTO(layout.position, value["position"].asInt());
        GEODE_UNWRAP_INTO(layout.color, value["color"].as<ccColor4B>());
        GEODE_UNWRAP_INTO(auto alignment, value["alignment"].asInt());
        layout.alignment = static_cast<CCTextAlignment>(alignment);
        GEODE_UNWRAP_INTO(layout.font, value["font"].asInt());
        GEODE_UNWRAP_INTO(layout.fontSize, value["fontSize"].asDouble());

        return Ok(layout);
    }

    static matjson::Value toJson(const LabelLayout& value) {
        matjson::Value obj = matjson::makeObject({
            { "labelName", value.labelName },
            { "text", value.text },
            { "line", value.line },
            { "position", value.position },
            { "color", value.color },
            { "alignment", static_cast<int>(value.alignment) },
            { "font", value.font },
            { "fontSize", value.fontSize }
        });
        return obj;
    }
};

class Save {
    public:
        static std::vector<LabelLayout> getLayout();
        static void setLayout(const std::vector<LabelLayout>& layout);

        static ccColor3B getNewBestColor();
        static void setNewBestColor(const ccColor3B& color);

        static ccColor3B getSessionBestColor();
        static void setSessionBestColor(const ccColor3B& color);

        static bool getExportWOutLabels();
        static void setExportWOutLabels(const bool& b);

        static std::string getLastOpenedVersion();
        static void setLastOpenedVersion(const std::string& verion);
};