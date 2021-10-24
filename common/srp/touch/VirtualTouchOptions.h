#ifndef __VIRTUALTOUCHOPTIONS_H__
#define __VIRTUALTOUCHOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace touch
{
    enum class VirtualTouchParameters
    {
        DeadzoneRadius,
        MaxFingerCount
    };

    static std::vector<VirtualTouchParameters> const virtualTouchParamsList
    {
        VirtualTouchParameters::DeadzoneRadius, VirtualTouchParameters::MaxFingerCount
    };

    static std::unordered_map<VirtualTouchParameters, std::string> const virtualTouchParamsDesc
    {
        { VirtualTouchParameters::DeadzoneRadius, "Deadzone Radius" },
        { VirtualTouchParameters::MaxFingerCount, "Max Finger Count" }
    };

    int16_t constexpr defaultVirtualTouchDeadzoneRadius = 5;
    int16_t constexpr minVirtualTouchDeadzoneRadius = 3;
    int16_t constexpr maxVirtualTouchDeadzoneRadius = 30;
    
    int16_t constexpr defaultMaxFingerCount = 2;
    int16_t constexpr minFingerCount = 1;
    int16_t constexpr maxFingerCount = 5;

    struct VirtualTouchConfig
    {
        int16_t deadzoneRadius;
        int16_t maxFingerCount;
    };

    int32_t constexpr virtualTouchConfigSize = sizeof(VirtualTouchConfig);

    std::unordered_map<VirtualTouchParameters, std::string> const virtualTouchParamsToStr(VirtualTouchConfig const config);
    VirtualTouchConfig const virtualTouchParamsFromStr(std::unordered_map<VirtualTouchParameters, std::string> const & map);
}

#endif