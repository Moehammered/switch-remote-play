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
    enum class ControllerParameters
    {
        Mode,
        ButtonMapping,
        LeftAnalogMapping,
        RightAnalogMapping,
        HomeButton,
        HomeButtonTriggerTime,
        ControllerCount
    };

    static std::vector<ControllerParameters> const controllerParamsList
    {
        ControllerParameters::Mode, ControllerParameters::ButtonMapping,
        ControllerParameters::LeftAnalogMapping, ControllerParameters::RightAnalogMapping,
        ControllerParameters::HomeButton, ControllerParameters::HomeButtonTriggerTime,
        ControllerParameters::ControllerCount
    };

    static std::unordered_map<ControllerParameters, std::string> const controllerParamsDesc
    {
        { ControllerParameters::Mode, "Controller Mode" },
        { ControllerParameters::ButtonMapping, "Controller Mapping" },
        { ControllerParameters::LeftAnalogMapping, "Left Analog Mapping" },
        { ControllerParameters::RightAnalogMapping, "Right Analog Mapping" },
        { ControllerParameters::HomeButton, "Home Button Key" },
        { ControllerParameters::HomeButtonTriggerTime, "Home Button Trigger Time" },
        { ControllerParameters::ControllerCount, "Controller Count" }
    };

    struct alignas(8) ControllerConfig
    {
        ControllerMode      controllerMode;
        ControllerButtonMap controllerMap;
        ControllerAnalogMap leftAnalogMap;
        ControllerAnalogMap rightAnalogMap;
        uint64_t            homeButton;
        uint64_t            homeButtonTriggerTime;
        int16_t             controllerCount;
        int8_t              padding[6];
    };

    auto constexpr controllerConfigSize = sizeof(controller::ControllerConfig);

    auto constexpr defaultMode = ControllerMode::X360;
    auto constexpr defaultButtonMap = ControllerButtonMap::None;
    auto constexpr defaultAnalogMap = ControllerAnalogMap::None;
    uint64_t constexpr defaultHomeButton = HidNpadButton_StickL;
    uint64_t constexpr minHomeButtonTriggerTime = timeutil::secondToNano(0.1);
    uint64_t constexpr maxHomeButtonTriggerTime = timeutil::secondToNano(4.0);
    uint64_t constexpr defaultHomeButtonTriggerTime = timeutil::secondToNano(1.0);

    int16_t constexpr defaultControllerCount = 1;
    int16_t constexpr maxControllerCount = 4;
    int16_t constexpr minControllerCount = 1;

    std::unordered_map<controller::ControllerParameters, std::string> const controllerParamsToStr(ControllerConfig const data);
    ControllerConfig const controllerParamsFromStr(std::unordered_map<controller::ControllerParameters, std::string> const & map);
}

#endif
