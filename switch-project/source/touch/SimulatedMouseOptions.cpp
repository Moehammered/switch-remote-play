#include "SimulatedMouseOptions.h"

namespace touch
{
    std::unordered_map<SimulatedTouchMouseParameters, std::string> const SimulatedTouchParamsToStr(SimulatedTouchConfig const config)
    {
        auto values = std::unordered_map<SimulatedTouchMouseParameters, std::string>{};

        values[SimulatedTouchMouseParameters::DeadzoneRadius] = std::to_string(config.deadzoneRadius);

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

        return config;
    }
}