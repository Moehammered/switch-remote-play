#include "VirtualTouchMouse.h"
#include <algorithm>
#include "DeadzoneUtility.h"

VirtualTouchMouse::VirtualTouchMouse(Resolution const screenExtents, int const coordinateMax, uint32_t deadzoneRadius)
    : virtualMouse{}, screenLimits{ screenExtents }, dimensionMax{ coordinateMax },
    deadzoneRadius{deadzoneRadius}, deadzoneMagSqr{magnitudeSqr(deadzoneRadius, deadzoneRadius)},
    lastX{}, lastY{}
{
}

void VirtualTouchMouse::Update(std::vector<VirtualFinger> const fingers)
{
    if (fingers.size() != 0)
    {
        virtualMouse.Press(SupportedMouseButtons::Left);

        auto finger = fingers[0];

        auto deadzoneExceeded = OutsideDeadzone(finger.x, finger.y, lastX, lastY);
        if (deadzoneExceeded)
        {
            auto scaledX = ScaleAbsolutePosition(finger.x, screenLimits.width, dimensionMax);
            auto scaledY = ScaleAbsolutePosition(finger.y, screenLimits.height, dimensionMax);

            virtualMouse.Move(SupportedMouseMovement::Absolute, scaledX, scaledY);
            lastX = finger.x;
            lastY = finger.y;
        }

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
        lastX = 0;
        lastY = 0;
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

bool VirtualTouchMouse::OutsideDeadzone(long x1, long y1, long x2, long y2)
{
    return outsideDeadzoneSqr(deadzoneMagSqr, x1, y1, x2, y2);
}
