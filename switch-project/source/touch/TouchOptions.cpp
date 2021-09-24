#include "TouchOptions.h"
#include "../utils/EnumMapper.h"

namespace touch
{
    TouchScreenMode TouchScreenModeStrToEnum(std::string s)
    {
        return strToEnum(TouchScreenModeStr, s);
    }

    std::string TouchScreenModeEnumToStr(TouchScreenMode mode)
    {
        return enumToStr(TouchScreenModeStr, mode);
    }

    std::string TouchScreenModeEnumToDesc(TouchScreenMode mode)
    {
        return enumToStr(TouchScreenModeDesc, mode);
    }

    std::unordered_map<TouchConfigParameters, std::string> const TouchConfigParamsToStr(TouchConfig const config)
    {
        auto values = std::unordered_map<TouchConfigParameters, std::string>{};

        values[TouchConfigParameters::TouchMode] = TouchScreenModeEnumToStr(config.touchMode);

        return values;
    }

    TouchConfig const TouchConfigParamsFromStr(std::unordered_map<TouchConfigParameters, std::string> const & map)
    {
        auto config = TouchConfig{};

        auto touchModeEntry = map.find(TouchConfigParameters::TouchMode);
        if(touchModeEntry != map.end())
        {
            auto mode = TouchScreenModeStrToEnum(touchModeEntry->second);
            config.touchMode = mode;
        }
        else
            config.touchMode = DefaultTouchScreenMode;
        
        switch(config.touchMode)
        {
            case TouchScreenMode::VirtualTouch:
                config.virtualTouchSettings = VirtualTouchConfig{};
                break;
            
            case TouchScreenMode::SimulatedMouse:
                config.simulatedTouchMouseSettings = SimulatedTouchConfig{};
                break;
        }

        return config;
    }
}