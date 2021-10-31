#ifndef __TOUCHSTREAM_H__
#define __TOUCHSTREAM_H__

#include "TouchInterface.h"
#include "VirtualDesktop.h"
#include "srp/touch/TouchOptions.h"
#include "srp/network/NetworkData.h"
#include <memory>

class TouchStream
{
public:
    TouchStream(touch::TouchConfig const config,
        VirtualDesktop const desktop,
        Resolution const switchResolution,
        DisplayDeviceInfo const sessionDisplay);

    void Process(InputDataPayload const& inputData, int64_t const nanoDelta);

    void Reset();

private:
    touch::TouchConfig const settings;
    VirtualDesktop const desktop;
    Resolution const switchResolution;
    DisplayDeviceInfo const sessionDisplay;
    std::unique_ptr<TouchInterface> const touch;
    int32_t const maxFingers;
    double const resolutionWidthRatio;
    double const resolutionHeightRatio;

    std::unique_ptr<TouchInterface> createVirtualTouchInterface(touch::TouchConfig const touchConfig, VirtualDesktop const desktop);
    int32_t const MaxFingerCount(touch::TouchConfig const config);
};

#endif
