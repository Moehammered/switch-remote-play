#include "SimulatedTrackpad.h"
#include "DeadzoneUtility.h"
#include "TimeUtil.h"
#include <chrono>

namespace
{
    auto doubleTapTimestamp = std::chrono::high_resolution_clock::now();
}

SimulatedTrackpad::SimulatedTrackpad(int deadzoneRadius, int32_t mouseSensitivity, double doubleTapTime)
    : deadzoneRadius{ deadzoneRadius }, deadzoneMagSqr{ deadzoneRadius * deadzoneRadius },
    mouseSensitivity{ mouseSensitivity }, doubleTapNanoTime{ (uint32_t)timeutil::secondToNano(doubleTapTime) },
    moved{ false }, mouse{},
    lastInitialFinger{}, lastFingerCount{ 0 }
{
    lastInitialFinger.id = UINT32_MAX;
    doubleTapTimestamp = std::chrono::high_resolution_clock::now();
}

void SimulatedTrackpad::Update(std::vector<VirtualFinger> const fingers)
{
    auto const now = std::chrono::high_resolution_clock::now();
    auto const delta = now - doubleTapTimestamp;
    //if no fingers, clear contact info
    if (fingers.size() == 0)
    {
        // release any buttons that may have been pressed
        if (lastFingerCount == 0)
        {
            mouse.Release(SupportedMouseButtons::Left);
            mouse.Release(SupportedMouseButtons::Right);
            moved = false;
        }
        else if (lastFingerCount == 1)
        {
            doubleTapTimestamp = now; // a finger's been released, timestamp it
            // press left mouse button
            if (!moved)
                mouse.Press(SupportedMouseButtons::Left);
            lastFingerCount = 0;
        }
        else if (lastFingerCount == 2)
        {
            if (!moved)
                mouse.Press(SupportedMouseButtons::Right);
            lastFingerCount = 0;
        }

        // reset last finger copy
        lastInitialFinger.id = UINT32_MAX;
    }
    else
    {
        // if 1 finger, it's a left click and/or drag action
        // if 2 fingers, it's a right click
        auto const& firstFinger = fingers[0];

        if (lastInitialFinger.id != UINT32_MAX && lastInitialFinger.id == firstFinger.id)
        {
            // check for movement
            // if there is movement, we need to make sure that left click is pressed
            auto lastPoint = Point<int64_t>{ lastInitialFinger.x, lastInitialFinger.y };
            auto currentPoint = Point<int64_t>{ firstFinger.x, firstFinger.y };
            if (outsideDeadzoneSqr(deadzoneMagSqr, lastPoint, currentPoint))
            {
                //press if a double tap has occurred within the buffer time
                if(delta.count() <= doubleTapNanoTime)
                    mouse.Press(SupportedMouseButtons::Left);

                auto deltaX = currentPoint.x - lastPoint.x;
                auto deltaY = currentPoint.y - lastPoint.y;

                mouse.Move((int)deltaX, (int)deltaY);

                lastInitialFinger = firstFinger;

                moved = true;
            }
            else
                mouse.Move(0, 0);
        }
        else
        {
            // newly pressed
            lastInitialFinger = firstFinger;
        }

        lastFingerCount = fingers.size();
    }
}

void SimulatedTrackpad::Release()
{
    auto const now = std::chrono::high_resolution_clock::now();
    auto const delta = now - doubleTapTimestamp;
    if (lastFingerCount == 0)
    {
        mouse.Release(SupportedMouseButtons::Left);
        mouse.Release(SupportedMouseButtons::Right);
        moved = false;
    }
    else if (lastFingerCount == 1)
    {
        doubleTapTimestamp = now; // a finger's been released, timestamp it
        // no finger movement occurred
        if (!moved)
            mouse.Press(SupportedMouseButtons::Left);
        lastFingerCount = 0;
    }
    else if (lastFingerCount == 2)
    {
        if (!moved)
            mouse.Press(SupportedMouseButtons::Right);
        lastFingerCount = 0;
    }

    // reset last finger copy
    lastInitialFinger.id = UINT32_MAX;
}

void SimulatedTrackpad::Commit()
{
    mouse.Commit();
}
