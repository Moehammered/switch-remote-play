#ifndef __SIMULATEDMOUSEOPTIONS_H__
#define __SIMULATEDMOUSEOPTIONS_H__

#include "SimulatedMouseBehaviour.h"
#include "../utils/TimeUtil.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace touch
{
    enum class SimulatedTouchMouseParameters
    {
        DeadzoneRadius,
        Behaviour,
        DoubleTapTime,
        TrackpadSensitivityPercentage
    };

    static std::vector<SimulatedTouchMouseParameters> const simulatedTouchParamsList
    {
        SimulatedTouchMouseParameters::DeadzoneRadius,
        SimulatedTouchMouseParameters::Behaviour,
        SimulatedTouchMouseParameters::DoubleTapTime,
        SimulatedTouchMouseParameters::TrackpadSensitivityPercentage
    };

    static std::unordered_map<SimulatedTouchMouseParameters, std::string> const simulatedTouchParamsDesc
    {
        { SimulatedTouchMouseParameters::DeadzoneRadius, "Deadzone Radius" },
        { SimulatedTouchMouseParameters::Behaviour, "Behaviour" },
        { SimulatedTouchMouseParameters::DoubleTapTime, "Double Tap Time" },
        { SimulatedTouchMouseParameters::TrackpadSensitivityPercentage, "Trackpad Sensitivity Percentage" }
    };

    int16_t constexpr defaultSimulatedMouseDeadzoneRadius = 5;
    int16_t constexpr minSimulatedMouseDeadzoneRadius = 3;
    int16_t constexpr maxSimulatedMouseDeadzoneRadius = 30;

    uint32_t constexpr defaultDoubleTapTime = (uint32_t)timeutil::secondToNano(0.2);
    uint32_t constexpr minDoubleTapTime = (uint32_t)timeutil::secondToNano(0.1);
    uint32_t constexpr maxDoubleTapTime = (uint32_t)timeutil::secondToNano(1);

    int16_t constexpr defaultTrackpadSensitivityPercentage = 85;
    int16_t constexpr minTrackpadSensitivityPercentage = 10;
    int16_t constexpr maxTrackpadSensitivityPercentage = 300;

    struct SimulatedTouchConfig
    {
        int16_t deadzoneRadius;
        SimulatedMouseBehaviour behaviour;
        uint32_t doubleTapTime;
        int16_t trackpadSensitivityPercentage;
    };

    int32_t constexpr simulatedTouchConfigSize = sizeof(SimulatedTouchConfig);

    std::unordered_map<SimulatedTouchMouseParameters, std::string> const simulatedTouchParamsToStr(SimulatedTouchConfig const config);
    SimulatedTouchConfig const simulatedTouchParamsFromStr(std::unordered_map<SimulatedTouchMouseParameters, std::string> const& map);
}

#endif
