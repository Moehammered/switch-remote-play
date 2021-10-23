#ifndef __SIMULATEDMOUSEOPTIONS_H__
#define __SIMULATEDMOUSEOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>
#include "SimulatedMouseBehaviour.h"
#include "TimeUtil.h"

namespace touch
{
    enum class SimulatedTouchMouseParameters
    {
        DeadzoneRadius,
        Behaviour,
        DoubleTapTime,
        TrackpadSensitivity
    };

    static std::vector<SimulatedTouchMouseParameters> const SimulatedTouchParamsList
    {
        SimulatedTouchMouseParameters::DeadzoneRadius,
        SimulatedTouchMouseParameters::Behaviour,
        SimulatedTouchMouseParameters::DoubleTapTime,
        SimulatedTouchMouseParameters::TrackpadSensitivity
    };

    static std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsDesc
    {
        { SimulatedTouchMouseParameters::DeadzoneRadius, "Deadzone Radius" },
        { SimulatedTouchMouseParameters::Behaviour, "Behaviour" },
        { SimulatedTouchMouseParameters::DoubleTapTime, "Double Tap Time" },
        { SimulatedTouchMouseParameters::TrackpadSensitivity, "Trackpad Sensitivity" }
    };

    int16_t constexpr DefaultSimulatedMouseDeadzoneRadius = 5;
    int16_t constexpr MinSimulatedMouseDeadzoneRadius = 3;
    int16_t constexpr MaxSimulatedMouseDeadzoneRadius = 30;

    uint32_t constexpr DefaultDoubleTapTime = (uint32_t)timeutil::secondToNano(0.2);
    uint32_t constexpr MinDoubleTapTime = (uint32_t)timeutil::secondToNano(0.1);
    uint32_t constexpr MaxDoubleTapTime = (uint32_t)timeutil::secondToNano(1);

    int16_t constexpr DefaultTrackpadSensitivityPercentage = 85;
    int16_t constexpr MinTrackpadSensitivityPercentage = 10;
    int16_t constexpr MaxTrackpadSensitivityPercentage = 300;

    struct SimulatedTouchConfig
    {
        int16_t deadzoneRadius;
        SimulatedMouseBehaviour behaviour;
        uint32_t doubleTapTime;
        int16_t trackpadSensitivityPercentage;
    };

    int32_t constexpr SimulatedTouchConfigSize = sizeof(SimulatedTouchConfig);

    std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsToStr(SimulatedTouchConfig const config);
    SimulatedTouchConfig const SimulatedTouchParamsFromStr(std::unordered_map<SimulatedTouchMouseParameters, std::string> const& map);
}

#endif
