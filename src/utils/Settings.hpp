#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Settings {
public:
    static int64_t getPrecision();
    static bool isCompletedLevelTrackingDisabled();
    static int64_t getMaxSessionLength();
};