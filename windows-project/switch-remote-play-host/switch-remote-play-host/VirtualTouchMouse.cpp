#include "VirtualTouchMouse.h"
#include <algorithm>

VirtualTouchMouse::VirtualTouchMouse(Resolution const screenExtents, int const coordinateMax)
    : virtualMouse{}, screenLimits{ screenExtents }, dimensionMax{ coordinateMax }
{
}

void VirtualTouchMouse::Update(std::vector<VirtualFinger> const fingers)
{
    if (fingers.size() != 0)
    {
        auto finger = fingers[0];

        auto scaledX = ScaleAbsolutePosition(finger.x, screenLimits.width, dimensionMax);
        auto scaledY = ScaleAbsolutePosition(finger.y, screenLimits.height, dimensionMax);

        virtualMouse.Press(SupportedMouseButtons::Left);
        virtualMouse.Move(SupportedMouseMovement::Absolute, scaledX, scaledY);

        if (fingers.size() == 2)
            virtualMouse.Press(SupportedMouseButtons::Right);
        else
            virtualMouse.Release(SupportedMouseButtons::Right);

        if (fingers.size() == 3)
            virtualMouse.Press(SupportedMouseButtons::Middle);
        else
            virtualMouse.Release(SupportedMouseButtons::Middle);

        virtualMouse.Commit();
    }
    else
    {
        virtualMouse.Release(SupportedMouseButtons::Left);
        virtualMouse.Release(SupportedMouseButtons::Middle);
        virtualMouse.Release(SupportedMouseButtons::Right);
        virtualMouse.Commit();
    }
}

void VirtualTouchMouse::Release()
{
    Update({});
}

int const VirtualTouchMouse::ScaleAbsolutePosition(int value, int dimension, int max)
{
    auto normalised = value / (double)dimension;
    auto scaled = (int)(normalised * max);

    return std::min(scaled, max);
}
