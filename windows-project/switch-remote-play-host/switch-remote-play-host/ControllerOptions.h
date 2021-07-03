#ifndef __CONTROLLEROPTIONS_H__
#define __CONTROLLEROPTIONS_H__

#include "SwitchButtons.h"
#include "ControllerMode.h"
#include "ControllerButtonMap.h"
#include "ControllerAnalogMap.h"
#include <vector>
#include <unordered_map>
#include "SwitchControlsDefinitions.h"

namespace controller
{
    enum class Parameters
    {
        Mode,
        ButtonMapping,
        LeftAnalogMapping,
        RightAnalogMapping,
        LeftMouseButton,
        RightMouseButton,
        MouseSensitivity,
        MouseOnConnect
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::Mode, Parameters::ButtonMapping,
        Parameters::LeftAnalogMapping, Parameters::RightAnalogMapping,
        Parameters::LeftMouseButton, Parameters::RightMouseButton,
        Parameters::MouseSensitivity, Parameters::MouseOnConnect
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::Mode, "Controller Mode" },
        { Parameters::ButtonMapping, "Controller Mapping" },
        { Parameters::LeftAnalogMapping, "Left Analog Mapping" },
        { Parameters::RightAnalogMapping, "Right Analog Mapping" },
        { Parameters::LeftMouseButton, "Left-Click Button" },
        { Parameters::RightMouseButton, "Right-Click Button" },
        { Parameters::MouseSensitivity, "Mouse Sensitivity" },
        { Parameters::MouseOnConnect, "Mouse on Connect" }
    };

    int32_t constexpr MaxMouseSensitity = 30;
    int32_t constexpr MinMouseSensitivity = 3;

    struct alignas(4) ControllerConfig
    {
        ControllerMode      controllerMode;
        ControllerButtonMap controllerMap;
        ControllerAnalogMap leftAnalogMap;
        ControllerAnalogMap rightAnalogMap;
        HidNpadButton       leftClickButton;
        HidNpadButton       rightClickButton;
        int16_t             mouseSensitivity;
        bool                mouseOnConnect;
        char                padding;
    };

    constexpr int ControllerConfigSize = sizeof(controller::ControllerConfig);

    std::unordered_map<controller::Parameters, std::string> const ControllerParamsToStr(ControllerConfig const data);
    ControllerConfig const ControllerParamsFromStr(std::unordered_map<controller::Parameters, std::string> const& map);
}

#endif
