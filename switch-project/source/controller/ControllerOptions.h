#ifndef __CONTROLLEROPTIONS_H__
#define __CONTROLLEROPTIONS_H__

#include "SwitchButtons.h"
#include "ControllerMode.h"
#include "ControllerButtonMap.h"
#include "ControllerAnalogMap.h"
#include <vector>
#include <unordered_map>

namespace controller
{
    enum class Parameters
    {
        Mode,
        ButtonMapping,
        LeftAnalogMapping,
        RightAnalogMapping,
        ControllerCount
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::Mode, Parameters::ButtonMapping,
        Parameters::LeftAnalogMapping, Parameters::RightAnalogMapping,
        Parameters::ControllerCount
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::Mode, "Controller Mode" },
        { Parameters::ButtonMapping, "Controller Mapping" },
        { Parameters::LeftAnalogMapping, "Left Analog Mapping" },
        { Parameters::RightAnalogMapping, "Right Analog Mapping" },
        { Parameters::ControllerCount, "Controller Count" }
    };

    struct alignas(4) ControllerConfig
    {
        ControllerMode      controllerMode;
        ControllerButtonMap controllerMap;
        ControllerAnalogMap leftAnalogMap;
        ControllerAnalogMap rightAnalogMap;
        int16_t             controllerCount;
    };

    constexpr int ControllerConfigSize = sizeof(controller::ControllerConfig);

    auto constexpr DefaultMode = ControllerMode::X360;
    auto constexpr DefaultButtonMap = ControllerButtonMap::None;
    auto constexpr DefaultAnalogMap = ControllerAnalogMap::None;

    int16_t constexpr DefaultControllerCount = 1;
    int16_t constexpr MaxControllerCount = 4;
    int16_t constexpr MinControllerCount = 1;

    std::unordered_map<controller::Parameters, std::string> const ControllerParamsToStr(ControllerConfig const data);
    ControllerConfig const ControllerParamsFromStr(std::unordered_map<controller::Parameters, std::string> const & map);
}

#endif
