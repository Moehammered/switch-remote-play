#include "ControllerOptions.h"
#include <algorithm>

namespace controller
{
    std::unordered_map<controller::Parameters, std::string> const ControllerParamsToStr(ControllerConfig const data)
    {
        auto values = std::unordered_map<controller::Parameters, std::string>{};

        auto boolToStr = [](bool val)
        {
            return val == true ? "yes" : "no";
        };

        values[Parameters::Mode] = ControllerModeToString(data.controllerMode);
        values[Parameters::ButtonMapping] = ControllerButtonMapToString(data.controllerMap);
        values[Parameters::LeftAnalogMapping] = ControllerAnalogMapToString(data.leftAnalogMap);
        values[Parameters::RightAnalogMapping] = ControllerAnalogMapToString(data.rightAnalogMap);
        values[Parameters::ControllerCount] = std::to_string(data.controllerCount);

        return values;
    }

    ControllerConfig const ControllerParamsFromStr(std::unordered_map<controller::Parameters, std::string> const& map)
    {
        auto data = ControllerConfig{};

        auto parse = [&](Parameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(Parameters::Mode, data.controllerMode, DefaultMode, ParseControllerModeString);
        parse(Parameters::ButtonMapping, data.controllerMap, DefaultButtonMap, ParseControllerButtonMapString);
        parse(Parameters::LeftAnalogMapping, data.leftAnalogMap, DefaultAnalogMap, ParseControllerAnalogMapString);
        parse(Parameters::RightAnalogMapping, data.rightAnalogMap, DefaultAnalogMap, ParseControllerAnalogMapString);

        {
            auto entry = map.find(Parameters::ControllerCount);
            if (entry != map.end())
            {
                auto str = entry->second;
                auto num = (int16_t)std::atoi(str.c_str());
                data.controllerCount = std::clamp(num, MinControllerCount, MaxControllerCount);
            }
            else
                data.controllerCount = DefaultControllerCount;
        }

        return data;
    }
}