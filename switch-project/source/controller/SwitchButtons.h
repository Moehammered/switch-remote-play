#ifndef __SWITCHBUTTONS_H__
#define __SWITCHBUTTONS_H__

#include <string>
#include <unordered_map>
#include <vector>
extern "C"
{
    #include <switch/services/hid.h>
}

namespace controller
{
    enum class AnalogStick
    {
        Left,
        Right
    };

    static std::unordered_map<AnalogStick, std::string> const analogStickOptions
    {
        { AnalogStick::Left, "Left Analog Stick" },
        { AnalogStick::Right, "Right Analog Stick" }
    };

    std::string AnalogStickToString(AnalogStick stick);
    AnalogStick ParseAnalogStickString(std::string s);

    std::string SwitchButtonToString(HidNpadButton btn);
    HidNpadButton ParseSwitchButtonString(std::string s);
    
    std::vector<std::string> const SwitchButtonsToString(uint64_t const keys);
    std::vector<HidNpadButton> const ParseSwitchButtonStrings(std::vector<std::string> const & strings);
    
    static std::unordered_map<HidNpadButton, std::string> const switchButtonOptions
    {
        { HidNpadButton::HidNpadButton_L, "L" },
        { HidNpadButton::HidNpadButton_R, "R" },
        { HidNpadButton::HidNpadButton_ZL, "ZL" },
        { HidNpadButton::HidNpadButton_ZR, "ZR" },
        { HidNpadButton::HidNpadButton_StickL, "Left Analog Button" },
        { HidNpadButton::HidNpadButton_StickR, "Right Analog Button" },
        { HidNpadButton::HidNpadButton_Up, "D-Pad Up" },
        { HidNpadButton::HidNpadButton_Down, "D-Pad Down" },
        { HidNpadButton::HidNpadButton_Left, "D-Pad Left" },
        { HidNpadButton::HidNpadButton_Right, "D-Pad Right" },
        { HidNpadButton::HidNpadButton_A, "A" },
        { HidNpadButton::HidNpadButton_B, "B" },
        { HidNpadButton::HidNpadButton_X, "X" },
        { HidNpadButton::HidNpadButton_Y, "Y" }
    };

    static std::unordered_map<HidNpadButton, std::string> const mouseButtonOptions
    {
        { HidNpadButton::HidNpadButton_L, "L" },
        { HidNpadButton::HidNpadButton_R, "R" },
        { HidNpadButton::HidNpadButton_ZL, "ZL" },
        { HidNpadButton::HidNpadButton_ZR, "ZR" },
        { HidNpadButton::HidNpadButton_StickL, "Left Analog Button" },
        { HidNpadButton::HidNpadButton_StickR, "Right Analog Button" }
    };

    HidNpadButton constexpr DefaultLeftMouseButton = HidNpadButton_R;
    HidNpadButton constexpr DefaultRightMouseButton = HidNpadButton_ZR;
}

#endif
