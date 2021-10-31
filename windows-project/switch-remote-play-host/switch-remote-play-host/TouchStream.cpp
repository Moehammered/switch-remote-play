#include "TouchStream.h"
#include "SimulatedTrackpad.h"
#include "AbsoluteTouchMouse.h"
#include "VirtualTouch.h"

TouchStream::TouchStream(touch::TouchConfig const config,
                        VirtualDesktop const desktop,
                        Resolution const switchResolution,
                        DisplayDeviceInfo const sessionDisplay)
    : settings{ config }, 
    desktop{ desktop },
    switchResolution{ switchResolution },
    sessionDisplay{ sessionDisplay },
    touch{ createVirtualTouchInterface(config, desktop) },
    maxFingers{ MaxFingerCount(config) },
    resolutionWidthRatio{ sessionDisplay.width / (double)switchResolution.width },
    resolutionHeightRatio{ sessionDisplay.height / (double)switchResolution.height }
{
}

void TouchStream::Process(InputDataPayload const& inputData, int64_t const nanoDelta)
{
    auto const& switchContactInfo = inputData.touchScreen;
    if (switchContactInfo.count > 0)
    {
        auto fingers = std::vector<VirtualFinger>{};
        for (auto i = 0; i < switchContactInfo.count && i < maxFingers; ++i)
        {
            auto finger = switchContactInfo.touches[i];
            auto reinterp = reinterpret_cast<VirtualFinger*>(&finger);
            fingers.emplace_back(*reinterp);
            auto x = fingers.back().x * resolutionWidthRatio;
            auto y = fingers.back().y * resolutionHeightRatio;
            fingers.back().x = x + sessionDisplay.x;
            fingers.back().y = y + sessionDisplay.y;
        }
        touch->Update(fingers);
        touch->Commit();
    }
    else
        Reset();
}

void TouchStream::Reset()
{
    touch->Release();
    touch->Commit();
}

std::unique_ptr<TouchInterface> TouchStream::createVirtualTouchInterface(touch::TouchConfig const touchConfig, VirtualDesktop const desktop)
{
    using namespace std;

    auto buildSimulatedMouse = [&]() -> unique_ptr<TouchInterface>
    {
        auto const data = touchConfig.simulatedTouchMouseSettings;
        auto doubleTapTime = timeutil::nanoToSecond(data.doubleTapTime);

        switch (data.behaviour)
        {
        default:
        case touch::SimulatedMouseBehaviour::Trackpad:
        {
            auto const trackpadSensitivity = data.trackpadSensitivityPercentage / 100.0;
            return make_unique<SimulatedTrackpad>(data.deadzoneRadius, trackpadSensitivity, doubleTapTime);
        }

        case touch::SimulatedMouseBehaviour::Absolute:
            return make_unique<AbsoluteTouchMouse>(data.deadzoneRadius, doubleTapTime, desktop);
        }
    };

    switch (touchConfig.touchMode)
    {
    default:
    case touch::TouchScreenMode::VirtualTouch:
    {
        auto const data = touchConfig.virtualTouchSettings;
        return make_unique<VirtualTouch>(data.deadzoneRadius, 2);
    }

    case touch::TouchScreenMode::SimulatedMouse:
        return buildSimulatedMouse();
    }
}

int32_t const TouchStream::MaxFingerCount(touch::TouchConfig const config)
{
    switch (config.touchMode)
    {
    case touch::TouchScreenMode::VirtualTouch:
    {
        auto const fingerCount = config.virtualTouchSettings.maxFingerCount;
        return std::min<int>(fingerCount, touch::maxFingerCount);
    }

    default:
    case touch::TouchScreenMode::SimulatedMouse:
        return touch::maxFingerCount;
    }
}
