#ifndef __MOUSEOPTIONS_H__
#define __MOUSEOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>
#include "../controller/SwitchButtons.h"

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
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::LeftMouseButton, Parameters::RightMouseButton,
        Parameters::MiddleMouseButton, Parameters::MouseSensitivity,
        Parameters::MouseOnConnect, Parameters::MouseWheelAnalog,
        Parameters::MouseModeToggleKey
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::LeftMouseButton, "Left-Click Button" },
        { Parameters::RightMouseButton, "Right-Click Button" },
        { Parameters::MiddleMouseButton, "Middle-Click Button" },
        { Parameters::MouseSensitivity, "Mouse Sensitivity" },
        { Parameters::MouseOnConnect, "Mouse on Connect" },
        { Parameters::MouseWheelAnalog, "Mouse Wheel Analog" },
        { Parameters::MouseModeToggleKey, "Mouse Mode Toggle Key" }
    };

    HidNpadButton constexpr DefaultLeftClickButton = HidNpadButton_R;
    HidNpadButton constexpr DefaultRightClickButton = HidNpadButton_ZR;
    HidNpadButton constexpr DefaultMiddleClickButton = HidNpadButton_StickR;
    int16_t constexpr MaxMouseSensitivity = 30;
    int16_t constexpr MinMouseSensitivity = 3;
    int16_t constexpr DefaultMouseSensitivity = 5;
    controller::AnalogStick constexpr DefaultMouseWheelAnalog = controller::AnalogStick::Right;
    uint32_t constexpr DefaultMouseModeToggleKey = HidNpadButton_ZL | HidNpadButton_ZR | HidNpadButton_B;

    struct alignas(4) MouseConfig
    {
        HidNpadButton               leftClickButton;
        HidNpadButton               rightClickButton;
        HidNpadButton               middleClickButton;
        int16_t                     mouseSensitivity;
        controller::AnalogStick     mouseWheelAnalog;
        uint32_t                    mouseModeToggleKey;
        bool                        mouseOnConnect;
    };
    
    int32_t constexpr MouseConfigSize = sizeof(MouseConfig);

    std::unordered_map<Parameters, std::string> const MouseParamsToStr(MouseConfig const config);
    MouseConfig const MouseParamsFromStr(std::unordered_map<Parameters, std::string> const & map);
}

#endif
