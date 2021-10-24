#ifndef __MOUSEOPTIONS_H__
#define __MOUSEOPTIONS_H__

#include "../controller/SwitchButtons.h"
#include "../utils/TimeUtil.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace mouse
{
    enum class MouseParameters
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

    static std::vector<MouseParameters> const mouseParamsList
    {
        MouseParameters::LeftMouseButton, MouseParameters::RightMouseButton,
        MouseParameters::MiddleMouseButton, MouseParameters::MouseSensitivity,
        MouseParameters::MouseOnConnect, MouseParameters::MouseWheelAnalog,
        MouseParameters::MouseModeToggleKey, MouseParameters::MouseModeToggleTime
    };

    static std::unordered_map<MouseParameters, std::string> const mouseParamsDesc
    {
        { MouseParameters::LeftMouseButton, "Left-Click Button" },
        { MouseParameters::RightMouseButton, "Right-Click Button" },
        { MouseParameters::MiddleMouseButton, "Middle-Click Button" },
        { MouseParameters::MouseSensitivity, "Mouse Sensitivity" },
        { MouseParameters::MouseOnConnect, "Mouse on Connect" },
        { MouseParameters::MouseWheelAnalog, "Mouse Wheel Analog" },
        { MouseParameters::MouseModeToggleKey, "Mouse Mode Toggle Key" },
        { MouseParameters::MouseModeToggleTime, "Mouse Mode Toggle Time" }
    };

    HidNpadButton constexpr defaultLeftClickButton = HidNpadButton_R;
    HidNpadButton constexpr defaultRightClickButton = HidNpadButton_ZR;
    HidNpadButton constexpr defaultMiddleClickButton = HidNpadButton_StickR;
    int16_t constexpr maxMouseSensitivity = 30;
    int16_t constexpr minMouseSensitivity = 3;
    int16_t constexpr defaultMouseSensitivity = 5;
    controller::AnalogStick constexpr defaultMouseWheelAnalog = controller::AnalogStick::Right;
    uint32_t constexpr defaultMouseModeToggleKey = HidNpadButton_Minus;
    uint32_t constexpr minMouseModeToggleTime = timeutil::secondToNano(0.1);
    uint32_t constexpr maxMouseModeToggleTime = timeutil::secondToNano(4);
    uint32_t constexpr defaultMouseModeToggleTime = timeutil::secondToNano(2);

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
    
    int32_t constexpr mouseConfigSize = sizeof(MouseConfig);

    std::unordered_map<MouseParameters, std::string> const mouseParamsToStr(MouseConfig const config);
    MouseConfig const mouseParamsFromStr(std::unordered_map<MouseParameters, std::string> const & map);
}

#endif
