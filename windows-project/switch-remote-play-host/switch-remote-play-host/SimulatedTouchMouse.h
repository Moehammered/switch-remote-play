#pragma once

#include <vector>
#include <unordered_map>
#include "VirtualFinger.h"
#include "VirtualMouse.h"

class SimulatedTouchMouse
{
public:
    SimulatedTouchMouse(int deadzoneRadius, int32_t mouseSensitivity, double doubleTapTime);

    void Update(std::vector<VirtualFinger> const fingers);
    void Release();

    void Commit();

private:
    int const deadzoneRadius;
    int const deadzoneMagSqr;
    int32_t const mouseSensitivity;
    uint32_t const doubleTapNanoTime;
    bool moved;

    VirtualMouse mouse;
    VirtualFinger lastInitialFinger;
    size_t lastFingerCount;
};