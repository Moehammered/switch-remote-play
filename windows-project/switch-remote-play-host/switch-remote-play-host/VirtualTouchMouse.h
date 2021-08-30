#pragma once
#include "VirtualMouse.h"
#include "Resolution.h"
#include "VirtualFinger.h"

class VirtualTouchMouse
{
public:
    VirtualTouchMouse(Resolution const screenExtents, int const coordinateMax, uint32_t deadzoneRadius);

    void Update(std::vector<VirtualFinger> const fingers);

    void Release();

private:
    VirtualMouse virtualMouse;
    Resolution const screenLimits;
    int const dimensionMax;
    uint32_t const deadzoneRadius;
    int64_t const deadzoneMagSqr;
    uint32_t lastX;
    uint32_t lastY;

    int const ScaleAbsolutePosition(int value, int dimension, int max);
    bool OutsideDeadzone(long x1, long y1, long x2, long y2);
};
