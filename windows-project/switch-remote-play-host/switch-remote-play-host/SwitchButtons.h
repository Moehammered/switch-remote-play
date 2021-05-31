#ifndef __SWITCHBUTTONS_H__
#define __SWITCHBUTTONS_H__

#include <string>
#include <unordered_map>
#include "SwitchControlsDefinitions.h"

namespace controller
{
    std::string SwitchButtonToString(HidNpadButton btn);
    HidNpadButton ParseSwitchButtonString(std::string s);
    
    static std::unordered_map<HidNpadButton, std::string> const switchButtonOptions
    {
        { HidNpadButton::HidNpadButton_L, "L" },
        { HidNpadButton::HidNpadButton_R, "R" },
        { HidNpadButton::HidNpadButton_ZL, "ZL" },
        { HidNpadButton::HidNpadButton_ZR, "ZR" }
    };

    static std::unordered_map<HidNpadButton, std::string> const mouseButtonOptions
    {
        { HidNpadButton::HidNpadButton_L, "L" },
        { HidNpadButton::HidNpadButton_R, "R" },
        { HidNpadButton::HidNpadButton_ZL, "ZL" },
        { HidNpadButton::HidNpadButton_ZR, "ZR" }
    };

    HidNpadButton constexpr DefaultLeftMouseButton = HidNpadButton_R;
    HidNpadButton constexpr DefaultRightMouseButton = HidNpadButton_ZR;
}

#endif
