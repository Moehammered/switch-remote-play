#ifndef __CONTROLLEROPTIONS_H__
#define __CONTROLLEROPTIONS_H__

#include "SwitchButtons.h"
#include "ControllerMode.h"
#include "ControllerButtonMap.h"
#include "ControllerAnalogMap.h"
#include "../utils/TimeUtil.h"
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
        HomeButton,
        HomeButtonTriggerTime,
        ControllerCount
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::Mode, Parameters::ButtonMapping,
        Parameters::LeftAnalogMapping, Parameters::RightAnalogMapping,
        Parameters::HomeButton, Parameters::HomeButtonTriggerTime,
        Parameters::ControllerCount
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::Mode, "Controller Mode" },
        { Parameters::ButtonMapping, "Controller Mapping" },
        { Parameters::LeftAnalogMapping, "Left Analog Mapping" },
        { Parameters::RightAnalogMapping, "Right Analog Mapping" },
        { Parameters::HomeButton, "Home Button Key" },
        { Parameters::HomeButtonTriggerTime, "Home Button Trigger Time" },
        { Parameters::ControllerCount, "Controller Count" }
    };

    struct alignas(4) ControllerConfig
    {
        ControllerMode      controllerMode;
        ControllerButtonMap controllerMap;
        ControllerAnalogMap leftAnalogMap;
        ControllerAnalogMap rightAnalogMap;
        uint32_t            homeButton;
        uint32_t            homeButtonTriggerTime;
        int16_t             controllerCount;
    };

    constexpr int ControllerConfigSize = sizeof(controller::ControllerConfig);

    auto constexpr DefaultMode = ControllerMode::X360;
    auto constexpr DefaultButtonMap = ControllerButtonMap::None;
    auto constexpr DefaultAnalogMap = ControllerAnalogMap::None;
    uint32_t constexpr DefaultHomeButton = HidNpadButton_StickL;
    uint32_t constexpr MinHomeButtonTriggerTime = timeutil::secondToNano(0.1);
    uint32_t constexpr MaxHomeButtonTriggerTime = timeutil::secondToNano(4.0);
    uint32_t constexpr DefaultHomeButtonTriggerTime = timeutil::secondToNano(1.0);

    int16_t constexpr DefaultControllerCount = 1;
    int16_t constexpr MaxControllerCount = 4;
    int16_t constexpr MinControllerCount = 1;

    std::unordered_map<controller::Parameters, std::string> const ControllerParamsToStr(ControllerConfig const data);
    ControllerConfig const ControllerParamsFromStr(std::unordered_map<controller::Parameters, std::string> const & map);
}

#endif
