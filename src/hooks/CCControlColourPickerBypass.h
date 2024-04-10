#pragma once

#include <Geode/Geode.hpp>

class CCControlColourPickerBypass : public CCControlColourPicker {
    public:
        ccColor3B getPickedColor() { return m_rgb;} 
};