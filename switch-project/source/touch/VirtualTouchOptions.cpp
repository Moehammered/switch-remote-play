#include "VirtualTouchOptions.h"

namespace touch
{
    std::unordered_map<VirtualTouchParameters, std::string> const virtualTouchParamsToStr(VirtualTouchConfig const config)
    {
        auto values = std::unordered_map<VirtualTouchParameters, std::string>{};

        values[VirtualTouchParameters::DeadzoneRadius] = std::to_string(config.deadzoneRadius);
        values[VirtualTouchParameters::MaxFingerCount] = std::to_string(config.maxFingerCount);

        return values;
    }

    VirtualTouchConfig const virtualTouchParamsFromStr(std::unordered_map<VirtualTouchParameters, std::string> const & map)
    {
        auto config = VirtualTouchConfig{};

        auto deadzoneEntry = map.find(VirtualTouchParameters::DeadzoneRadius);
        if(deadzoneEntry != map.end())
        {
            auto deadzone = (int16_t)std::stoi(deadzoneEntry->second);
            deadzone = std::min(deadzone, maxVirtualTouchDeadzoneRadius);
            deadzone = std::max(deadzone, minVirtualTouchDeadzoneRadius);
            config.deadzoneRadius = deadzone;
        }
        else
            config.deadzoneRadius = defaultVirtualTouchDeadzoneRadius;

        auto fingerCountEntry = map.find(VirtualTouchParameters::MaxFingerCount);
        if(fingerCountEntry != map.end())
        {
            auto fingerCount = (int16_t)std::stoi(fingerCountEntry->second);
            fingerCount = std::min(fingerCount, maxFingerCount);
            fingerCount = std::max(fingerCount, minFingerCount);
            config.maxFingerCount = fingerCount;
        }
        else
            config.maxFingerCount = defaultMaxFingerCount;
        

        return config;
    }
}