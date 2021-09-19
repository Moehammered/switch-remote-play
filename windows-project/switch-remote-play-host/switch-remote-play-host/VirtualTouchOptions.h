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

    static std::vector<VirtualTouchParameters> const VirtualTouchParamsList
    {
        VirtualTouchParameters::DeadzoneRadius, VirtualTouchParameters::MaxFingerCount
    };

    static std::unordered_map<VirtualTouchParameters, std::string> const VirtualTouchParamsDesc
    {
        { VirtualTouchParameters::DeadzoneRadius, "Deadzone Radius" },
        { VirtualTouchParameters::MaxFingerCount, "Max Finger Count" }
    };

    int16_t constexpr DefaultVirtualTouchDeadzoneRadius = 5;
    int16_t constexpr MinVirtualTouchDeadzoneRadius = 3;
    int16_t constexpr MaxVirtualTouchDeadzoneRadius = 30;
    
    int16_t constexpr DefaultMaxFingerCount = 2;
    int16_t constexpr MinFingerCount = 1;
    int16_t constexpr MaxFingerCount = 5;

    struct VirtualTouchConfig
    {
        int16_t deadzoneRadius;
        int16_t maxFingerCount;
    };

    int32_t constexpr VirtualTouchConfigSize = sizeof(VirtualTouchConfig);

    std::unordered_map<VirtualTouchParameters, std::string> const VirtualTouchParamsToStr(VirtualTouchConfig const config);
    VirtualTouchConfig const VirtualTouchParamsFromStr(std::unordered_map<VirtualTouchParameters, std::string> const & map);
}

#endif