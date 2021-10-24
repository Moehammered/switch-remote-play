#include "TouchOptions.h"
#include "../utils/EnumMapper.h"

namespace touch
{
    TouchScreenMode touchScreenModeFromStr(std::string s)
    {
        return strToEnum(touchScreenModeStr, s);
    }

    std::string touchScreenModeEnumToStr(TouchScreenMode mode)
    {
        return enumToStr(touchScreenModeStr, mode);
    }

    std::string touchScreenModeEnumToDesc(TouchScreenMode mode)
    {
        return enumToStr(touchScreenModeDesc, mode);
    }

    std::unordered_map<TouchConfigParameters, std::string> const touchConfigParamsToStr(TouchConfig const config)
    {
        auto values = std::unordered_map<TouchConfigParameters, std::string>{};

        values[TouchConfigParameters::TouchMode] = touchScreenModeEnumToStr(config.touchMode);

        return values;
    }

    TouchConfig const touchConfigParamsFromStr(std::unordered_map<TouchConfigParameters, std::string> const & map)
    {
        auto config = TouchConfig{};

        auto touchModeEntry = map.find(TouchConfigParameters::TouchMode);
        if(touchModeEntry != map.end())
        {
            auto mode = touchScreenModeFromStr(touchModeEntry->second);
            config.touchMode = mode;
        }
        else
            config.touchMode = defaultTouchScreenMode;
        
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