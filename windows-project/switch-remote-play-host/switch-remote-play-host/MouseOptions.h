#ifndef __MOUSEOPTIONS_H__
#define __MOUSEOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>
#include "SwitchControlsDefinitions.h"
#include "SwitchButtons.h"
#include "TimeUtil.h"

namespace mouse
{
    enum class Parameters
    {
        LeftMouseButton,
        RightMouseButton,
        MiddleMouseButton,
        MouseSensitivity,
        MouseOnConnect,
        MouseWheelAnalog,
        MouseModeToggleKey,
        MouseModeToggleTime
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::LeftMouseButton, Parameters::RightMouseButton,
        Parameters::MiddleMouseButton, Parameters::MouseSensitivity,
        Parameters::MouseOnConnect, Parameters::MouseWheelAnalog,
        Parameters::MouseModeToggleKey, Parameters::MouseModeToggleTime
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::LeftMouseButton, "Left-Click Button" },
        { Parameters::RightMouseButton, "Right-Click Button" },
        { Parameters::MiddleMouseButton, "Middle-Click Button" },
        { Parameters::MouseSensitivity, "Mouse Sensitivity" },
        { Parameters::MouseOnConnect, "Mouse on Connect" },
        { Parameters::MouseWheelAnalog, "Mouse Wheel Analog" },
        { Parameters::MouseModeToggleKey, "Mouse Mode Toggle Key" },
        { Parameters::MouseModeToggleTime, "Mouse Mode Toggle Time" }
    };

    HidNpadButton constexpr DefaultLeftClickButton = HidNpadButton_R;
    HidNpadButton constexpr DefaultRightClickButton = HidNpadButton_ZR;
    HidNpadButton constexpr DefaultMiddleClickButton = HidNpadButton_StickR;
    int16_t constexpr MaxMouseSensitivity = 30;
    int16_t constexpr MinMouseSensitivity = 3;
    int16_t constexpr DefaultMouseSensitivity = 5;
    controller::AnalogStick constexpr DefaultMouseWheelAnalog = controller::AnalogStick::Right;
    uint32_t constexpr DefaultMouseModeToggleKey = HidNpadButton_Minus;
    uint32_t constexpr MinMouseModeToggleTime = timeutil::secondToNano(0.1);
    uint32_t constexpr MaxMouseModeToggleTime = timeutil::secondToNano(4);
    uint32_t constexpr DefaultMouseModeToggleTime = timeutil::secondToNano(2);

    struct alignas(4) MouseConfig
    {
        HidNpadButton               leftClickButton;
        HidNpadButton               rightClickButton;
        HidNpadButton               middleClickButton;
        int16_t                     mouseSensitivity;
        controller::AnalogStick     mouseWheelAnalog;
        uint32_t                    mouseModeToggleKey;
        uint32_t                    mouseModeToggleTime;
        bool                        mouseOnConnect;
    };
    
    int32_t constexpr MouseConfigSize = sizeof(MouseConfig);

    std::unordered_map<Parameters, std::string> const MouseParamsToStr(MouseConfig const config);
    MouseConfig const MouseParamsFromStr(std::unordered_map<Parameters, std::string> const & map);
}

#endif
