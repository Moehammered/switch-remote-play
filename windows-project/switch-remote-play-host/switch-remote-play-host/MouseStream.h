#ifndef __MOUSESTREAM_H__
#define __MOUSESTREAM_H__

#include "VirtualMouse.h"
#include "srp/mouse/MouseOptions.h"
#include "srp/network/NetworkData.h"

class MouseStream
{
public:
    MouseStream(mouse::MouseConfig const config, double const joystickMax, double const wheelClickDelta);

    void Process(InputDataPayload const& inputData, int64_t const nanoDelta);

    void Reset();

private:
    mouse::MouseConfig const settings;
    VirtualMouse mouse;
    double const joystickExtent;
    double const wheelDelta;

    void ProcessMouseMovement(VirtualMouse& mouse,
        GamepadDataPayload const& padData,
        int16_t mouseSensitivity,
        controller::AnalogStick mouseWheelAnalog);

    void ProcessMouseButtons(VirtualMouse& mouse,
        GamepadDataPayload const& padData,
        HidNpadButton leftClickBtn,
        HidNpadButton rightClickBtn,
        HidNpadButton middleClickBtn);
};

#endif
