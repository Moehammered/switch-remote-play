#ifndef __TOUCHOPTIONS_H__
#define __TOUCHOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>
#include "VirtualTouchOptions.h"
#include "SimulatedMouseOptions.h"

namespace touch
{
    enum class TouchScreenMode
    {
        VirtualTouch,
        SimulatedMouse
    };

    static std::vector<TouchScreenMode> const TouchScreenModeList
    {
        TouchScreenMode::VirtualTouch, TouchScreenMode::SimulatedMouse
    };

    static std::unordered_map<TouchScreenMode, std::string> const TouchScreenModeStr
    {
        { TouchScreenMode::VirtualTouch, "virtual touch" },
        { TouchScreenMode::SimulatedMouse, "simulated mouse" }
    };

    static std::unordered_map<TouchScreenMode, std::string> const TouchScreenModeDesc
    {
        { TouchScreenMode::VirtualTouch, "Virtual Touch (PC behaves like a touch device)" },
        { TouchScreenMode::SimulatedMouse, "Simulated Mouse (Touch screen behaves like a mouse)" }
    };

    TouchScreenMode TouchScreenModeStrToEnum(std::string s);
    std::string TouchScreenModeEnumToStr(TouchScreenMode mode);
    std::string TouchScreenModeEnumToDesc(TouchScreenMode mode);

    TouchScreenMode constexpr DefaultTouchScreenMode = TouchScreenMode::VirtualTouch;

    struct TouchConfig
    {
        TouchScreenMode touchMode;
        union
        {
            VirtualTouchConfig      virtualTouchSettings;
            SimulatedTouchConfig    simulatedTouchMouseSettings;
        };
    };

    int32_t constexpr TouchConfigSize = sizeof(TouchConfig);
}

#endif
