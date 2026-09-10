#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DateFormatter {
public:
    static std::string timeFormat(const tm& dateTime);
};