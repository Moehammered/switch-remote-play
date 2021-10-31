#include "MouseStream.h"

MouseStream::MouseStream(mouse::MouseConfig const config, 
    double const joystickMax, 
    double const wheelClickDelta)
    : settings{ config }, mouse{}, 
    joystickExtent{ joystickMax }, wheelDelta{ wheelClickDelta }
{
}

void MouseStream::Process(InputDataPayload const& inputData, int64_t const nanoDelta)
{
    auto const mouseSensitivity = settings.mouseSensitivity;
    auto const mouseWheelAnalog = settings.mouseWheelAnalog;
    ProcessMouseMovement(mouse, inputData.gamepads[0], mouseSensitivity, mouseWheelAnalog);
    auto const leftClickBtn = settings.leftClickButton;
    auto const rightClickBtn = settings.rightClickButton;
    auto const middleClickBtn = settings.middleClickButton;
    ProcessMouseButtons(mouse, inputData.gamepads[0], leftClickBtn, rightClickBtn, middleClickBtn);
    mouse.Commit();
}

void MouseStream::Reset()
{
    mouse.Release(SupportedMouseButtons::Left);
    mouse.Commit();
    mouse.Release(SupportedMouseButtons::Right);
    mouse.Commit();
    mouse.Release(SupportedMouseButtons::Middle);
    mouse.Commit();
}

void MouseStream::ProcessMouseMovement(VirtualMouse& mouse,
    GamepadDataPayload const& padData, 
    int16_t mouseSensitivity, 
    controller::AnalogStick mouseWheelAnalog)
{
    auto cursorAnalogX = 0;
    auto cursorAnalogY = 0;
    auto mouseWheelAnalogX = 0;
    auto mouseWheelAnalogY = 0;

    if (mouseWheelAnalog == controller::AnalogStick::Right)
    {
        cursorAnalogX = padData.ljx;
        cursorAnalogY = padData.ljy;
        mouseWheelAnalogX = padData.rjx;
        mouseWheelAnalogY = padData.rjy;
    }
    else
    {
        cursorAnalogX = padData.rjx;
        cursorAnalogY = padData.rjy;
        mouseWheelAnalogX = padData.ljx;
        mouseWheelAnalogY = padData.ljy;
    }

    auto x = cursorAnalogX % (int)joystickExtent;
    auto y = (-cursorAnalogY) % (int)joystickExtent;
    auto normalisedX = (long)(mouseSensitivity * (x / joystickExtent));
    auto normalisedY = (long)(mouseSensitivity * (y / joystickExtent));
    mouse.Move(normalisedX, normalisedY);

    auto const maxScrollSpeed = 0.5 * wheelDelta;
    auto normalisedScrX = mouseWheelAnalogX / joystickExtent;
    auto normalisedScrY = mouseWheelAnalogY / joystickExtent;
    auto scrDeltaX = (int)(normalisedScrX * maxScrollSpeed);
    auto scrDeltaY = (int)(normalisedScrY * maxScrollSpeed);
    mouse.ScrollWheel(scrDeltaX, scrDeltaY);
}

void MouseStream::ProcessMouseButtons(VirtualMouse& mouse, 
    GamepadDataPayload const& padData, 
    HidNpadButton leftClickBtn, 
    HidNpadButton rightClickBtn, 
    HidNpadButton middleClickBtn)
{
    if (padData.keys & leftClickBtn)
        mouse.Press(SupportedMouseButtons::Left);
    else
        mouse.Release(SupportedMouseButtons::Left);

    if (padData.keys & rightClickBtn)
        mouse.Press(SupportedMouseButtons::Right);
    else
        mouse.Release(SupportedMouseButtons::Right);

    if (padData.keys & middleClickBtn)
        mouse.Press(SupportedMouseButtons::Middle);
    else
        mouse.Release(SupportedMouseButtons::Middle);
}
