#ifndef __CONTROLLERSTREAM_H__
#define __CONTROLLERSTREAM_H__

#include "srp/controller/ControllerOptions.h"
#include "VirtualController.h"
#include <vector>
#include <memory>

class ControllerStream
{
public:
    ControllerStream(controller::ControllerConfig const config);

    void Process(InputDataPayload const& inputData, int64_t const nanoDelta);

    void Reset();

private:
    controller::ControllerConfig const settings;
    std::vector<std::unique_ptr<IVirtualController>> controllers;
    std::vector<long long> homeTriggerTimers;

    std::vector<std::unique_ptr<IVirtualController>> CreateVirtualControllers(controller::ControllerConfig const config);

    void ProcessGamepad(std::unique_ptr<IVirtualController> const& controller,
        GamepadDataPayload padData,
        uint32_t const homeHeldBtn,
        HidNpadButton const homeBtn,
        long long const homeTriggerTimeNano,
        long long& homeTriggerTimer,
        long long const deltaTime);

    bool GamepadHasInput(GamepadDataPayload const& pad);
};

#endif
