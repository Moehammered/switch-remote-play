#include "ControllerOptions.h"

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

        parse(Parameters::Mode, data.controllerMode, ControllerMode::X360, ParseControllerModeString);
        parse(Parameters::ButtonMapping, data.controllerMap, ControllerButtonMap::None, ParseControllerButtonMapString);
        parse(Parameters::LeftAnalogMapping, data.leftAnalogMap, ControllerAnalogMap::None, ParseControllerAnalogMapString);
        parse(Parameters::RightAnalogMapping, data.rightAnalogMap, ControllerAnalogMap::None, ParseControllerAnalogMapString);

        return data;
    }
}