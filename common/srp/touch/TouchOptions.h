#ifndef __TOUCHOPTIONS_H__
#define __TOUCHOPTIONS_H__

#include "VirtualTouchOptions.h"
#include "SimulatedMouseOptions.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace touch
{
    enum class TouchScreenMode
    {
        VirtualTouch,
        SimulatedMouse
    };

    static std::vector<TouchScreenMode> const touchScreenModeList
    {
        TouchScreenMode::VirtualTouch, TouchScreenMode::SimulatedMouse
    };

    static std::unordered_map<TouchScreenMode, std::string> const touchScreenModeStr
    {
        { TouchScreenMode::VirtualTouch, "Virtual Touch" },
        { TouchScreenMode::SimulatedMouse, "Simulated Mouse" }
    };

    static std::unordered_map<TouchScreenMode, std::string> const touchScreenModeDesc
    {
        { TouchScreenMode::VirtualTouch, "Virtual Touch (PC behaves like a touch device)" },
        { TouchScreenMode::SimulatedMouse, "Simulated Mouse (Touch screen behaves like a mouse)" }
    };

    TouchScreenMode touchScreenModeFromStr(std::string s);
    std::string touchScreenModeEnumToStr(TouchScreenMode mode);
    std::string touchScreenModeEnumToDesc(TouchScreenMode mode);

    auto constexpr defaultTouchScreenMode = TouchScreenMode::VirtualTouch;

    struct TouchConfig
    {
        TouchScreenMode touchMode;
        union
        {
            VirtualTouchConfig      virtualTouchSettings;
            SimulatedTouchConfig    simulatedTouchMouseSettings;
        };
    };

    int32_t constexpr touchConfigSize = sizeof(TouchConfig);

    enum class TouchConfigParameters
    {
        TouchMode
    };

    static std::vector<TouchConfigParameters> const touchConfigParamsList
    {
        TouchConfigParameters::TouchMode
    };

    static std::unordered_map<TouchConfigParameters, std::string> const touchConfigParamsDesc
    {
        { TouchConfigParameters::TouchMode, "Touch Mode" }
    };

    std::unordered_map<TouchConfigParameters, std::string> const touchConfigParamsToStr(TouchConfig const config);
    TouchConfig const touchConfigParamsFromStr(std::unordered_map<TouchConfigParameters, std::string> const & map);
}

#endif
