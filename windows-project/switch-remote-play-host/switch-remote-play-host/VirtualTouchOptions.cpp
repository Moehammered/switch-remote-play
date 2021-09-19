#include "VirtualTouchOptions.h"

namespace touch
{
    std::unordered_map<VirtualTouchParameters, std::string> const VirtualTouchParamsToStr(VirtualTouchConfig const config)
    {
        auto values = std::unordered_map<VirtualTouchParameters, std::string>{};

        values[VirtualTouchParameters::DeadzoneRadius] = std::to_string(config.deadzoneRadius);
        values[VirtualTouchParameters::MaxFingerCount] = std::to_string(config.maxFingerCount);

        return values;
    }

    VirtualTouchConfig const VirtualTouchParamsFromStr(std::unordered_map<VirtualTouchParameters, std::string> const & map)
    {
        auto config = VirtualTouchConfig{};

        auto deadzoneEntry = map.find(VirtualTouchParameters::DeadzoneRadius);
        if(deadzoneEntry != map.end())
        {
            auto deadzone = (int16_t)std::stoi(deadzoneEntry->second);
            deadzone = std::min(deadzone, MaxVirtualTouchDeadzoneRadius);
            deadzone = std::max(deadzone, MinVirtualTouchDeadzoneRadius);
            config.deadzoneRadius = deadzone;
        }
        else
            config.deadzoneRadius = DefaultVirtualTouchDeadzoneRadius;

        auto fingerCountEntry = map.find(VirtualTouchParameters::MaxFingerCount);
        if(fingerCountEntry != map.end())
        {
            auto fingerCount = (int16_t)std::stoi(fingerCountEntry->second);
            fingerCount = std::min(fingerCount, MaxFingerCount);
            fingerCount = std::max(fingerCount, MinFingerCount);
            config.maxFingerCount = fingerCount;
        }
        else
            config.maxFingerCount = DefaultMaxFingerCount;
        

        return config;
    }
}