#include "ControllerStream.h"
#include "X360Controller.h"
#include "DS4Controller.h"
#include <algorithm>

ControllerStream::ControllerStream(controller::ControllerConfig const config)
    : settings{ config }, 
    controllers{ std::move(CreateVirtualControllers(config)) },
    homeTriggerTimers(controllers.size())
{
}

void ControllerStream::Process(InputDataPayload const& inputData, int64_t const nanoDelta)
{
    auto const homeBtn = HidNpadButton::HidNpadButton_Palma;
    auto const homeTriggerTimeNano = settings.homeButtonTriggerTime;

    for (auto i = 0U; i < controllers.size(); ++i)
    {
        auto const& controller = controllers[i];
        auto& padData = inputData.gamepads[i];
        auto& homeTriggerTimer = homeTriggerTimers[i];

        ProcessGamepad(controller, padData,
            settings.homeButton, homeBtn,
            homeTriggerTimeNano, homeTriggerTimer,
            nanoDelta);
    }
}

void ControllerStream::Reset()
{
    for (auto& controller : controllers)
    {
        controller->ResetController();
        controller->UpdateController();
    }
}

void ControllerStream::DisconnectControllers()
{
    for (auto& controller : controllers)
        controller->Disconnect();
}

std::vector<std::unique_ptr<IVirtualController>> ControllerStream::CreateVirtualControllers(controller::ControllerConfig const config)
{
    using namespace std;
    auto controllerCount = clamp(config.controllerCount, controller::minControllerCount, controller::maxControllerCount);
    auto controllers = vector<unique_ptr<IVirtualController>>(controllerCount);

    for (auto& controller : controllers)
    {
        switch (config.controllerMode)
        {
        default:
        case controller::ControllerMode::X360:
            controller = make_unique<X360Controller>();
            break;

        case controller::ControllerMode::DS4:
            controller = make_unique<DS4Controller>();
            break;
        }

        if (controller->Create())
        {
            controller->MapFaceButtons(config.controllerMap);
            controller->MapAnalogAxis(config.leftAnalogMap, config.rightAnalogMap);
        }
    }

    return controllers;
}

void ControllerStream::ProcessGamepad(std::unique_ptr<IVirtualController> const& controller,
    GamepadDataPayload padData, 
    uint32_t const homeHeldBtn, 
    HidNpadButton const homeBtn, 
    long long const homeTriggerTimeNano, 
    long long& homeTriggerTimer, 
    long long const deltaTime)
{
    if (GamepadHasInput(padData))
    {
        auto homeTriggerMask = padData.keys & homeHeldBtn;
        if (homeTriggerMask == homeHeldBtn)
        {
            homeTriggerTimer += deltaTime;
            if (homeTriggerTimer > homeTriggerTimeNano)
            {
                auto keysCopy = padData.keys & ~homeHeldBtn;
                padData.keys = keysCopy | homeBtn;
            }
        }
        else
            homeTriggerTimer = 0;

        controller->ConvertPayload(padData);
        //controller->Print();
        controller->UpdateState();
        controller->UpdateController();
    }
    else
    {
        homeTriggerTimer = 0;
        controller->ResetController();
        controller->UpdateController();
    }
}

bool ControllerStream::GamepadHasInput(GamepadDataPayload const& pad)
{
    return pad.keys != 0x0 || (pad.ljx | pad.ljy | pad.rjx | pad.rjy) != 0;
}