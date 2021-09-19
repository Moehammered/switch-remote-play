#ifndef __SIMULATEDMOUSEOPTIONS_H__
#define __SIMULATEDMOUSEOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace touch
{
    enum class SimulatedTouchMouseParameters
    {
        DeadzoneRadius
    };

    static std::vector<SimulatedTouchMouseParameters> const SimulatedTouchParamsList
    {
        SimulatedTouchMouseParameters::DeadzoneRadius
    };

    static std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsDesc
    {
        { SimulatedTouchMouseParameters::DeadzoneRadius, "Deadzone Radius" }
    };

    int16_t constexpr DefaultSimulatedMouseDeadzoneRadius = 5;
    int16_t constexpr MinSimulatedMouseDeadzoneRadius = 3;
    int16_t constexpr MaxSimulatedMouseDeadzoneRadius = 30;

    struct SimulatedTouchConfig
    {
        int16_t deadzoneRadius;
    };

    int32_t constexpr SimulatedTouchConfigSize = sizeof(SimulatedTouchConfig);

    std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsToStr(SimulatedTouchConfig const config);
    SimulatedTouchConfig const SimulatedTouchParamsFromStr(std::unordered_map<SimulatedTouchMouseParameters, std::string> const & map);
}

#endif
