#include "SimulatedMouseOptions.h"
#include <algorithm>

namespace touch
{
    std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsToStr(SimulatedTouchConfig const config)
    {
        auto values = std::unordered_map<SimulatedTouchMouseParameters, std::string>{};

        values[SimulatedTouchMouseParameters::DeadzoneRadius] = std::to_string(config.deadzoneRadius);
        values[SimulatedTouchMouseParameters::Behaviour] = SimulatedMouseBehaviourToStr(config.behaviour);

        auto const triggerTimeSeconds = timeutil::nanoToSecond(config.doubleTapTime);
        auto secondsStr = std::to_string(triggerTimeSeconds);
        values[SimulatedTouchMouseParameters::DoubleTapTime] = secondsStr;

        values[SimulatedTouchMouseParameters::TrackpadSensitivityPercentage] = std::to_string(config.trackpadSensitivityPercentage);

        return values;
    }

    SimulatedTouchConfig const SimulatedTouchParamsFromStr(std::unordered_map<SimulatedTouchMouseParameters, std::string> const & map)
    {
        auto config = SimulatedTouchConfig{};

        auto deadzoneEntry = map.find(SimulatedTouchMouseParameters::DeadzoneRadius);
        if(deadzoneEntry != map.end())
        {
            auto deadzone = (int16_t)std::stoi(deadzoneEntry->second);
            deadzone = std::min(deadzone, MaxSimulatedMouseDeadzoneRadius);
            deadzone = std::max(deadzone, MinSimulatedMouseDeadzoneRadius);
            config.deadzoneRadius = deadzone;
        }
        else
            config.deadzoneRadius = DefaultSimulatedMouseDeadzoneRadius;

        {
            auto entry = map.find(SimulatedTouchMouseParameters::Behaviour);
            if(entry != map.end())
            {
                auto behaviour = SimulatedMouseBehaviourFromStr(entry->second);
                config.behaviour = behaviour;
            }
            else
                config.behaviour = DefaultMouseBehaviour;
        }

        {
            auto entry = map.find(SimulatedTouchMouseParameters::DoubleTapTime);
            if(entry != map.end())
            {
                auto secondsStr = entry->second;
                auto const seconds = std::atof(secondsStr.c_str());
                auto const nanoseconds = (uint32_t)timeutil::secondToNano(seconds);
                config.doubleTapTime = std::clamp(nanoseconds, MinDoubleTapTime, MaxDoubleTapTime);
            }
            else
                config.doubleTapTime = DefaultDoubleTapTime;
        }

        {
            auto entry = map.find(SimulatedTouchMouseParameters::TrackpadSensitivityPercentage);
            if(entry != map.end())
            {
                auto sensitivityStr = entry->second;
                auto const sensitivity = (int16_t)std::atoi(sensitivityStr.c_str());
                config.trackpadSensitivityPercentage = std::clamp(sensitivity, MinTrackpadSensitivityPercentage, MaxTrackpadSensitivityPercentage);
            }
            else
                config.trackpadSensitivityPercentage = DefaultTrackpadSensitivityPercentage;
        }

        return config;
    }
}