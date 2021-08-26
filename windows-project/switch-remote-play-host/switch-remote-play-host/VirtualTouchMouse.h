#pragma once
#include "VirtualMouse.h"
#include "Resolution.h"
#include "VirtualFinger.h"

class VirtualTouchMouse
{
public:
    VirtualTouchMouse(Resolution const screenExtents, int const coordinateMax);

    void Update(std::vector<VirtualFinger> const fingers);

    void Release();

private:
    VirtualMouse virtualMouse;
    Resolution const screenLimits;
    int const dimensionMax;

    int const ScaleAbsolutePosition(int value, int dimension, int max);
};
