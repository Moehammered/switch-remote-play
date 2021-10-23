#pragma once

#include <vector>
#include <unordered_map>
#include "VirtualFinger.h"
#include "VirtualMouse.h"
#include "TouchInterface.h"

class SimulatedTrackpad : public TouchInterface
{
public:
    SimulatedTrackpad(int deadzoneRadius, double mouseSensitivity, double doubleTapTime);

    void Update(std::vector<VirtualFinger> const fingers) override;
    void Release() override;

    void Commit() override;

private:
    int const deadzoneRadius;
    int const deadzoneMagSqr;
    double const mouseSensitivity;
    uint32_t const doubleTapNanoTime;
    bool moved;

    VirtualMouse mouse;
    VirtualFinger lastInitialFinger;
    size_t lastFingerCount;
};