#include "AbsoluteTouchMouse.h"
#include "DeadzoneUtility.h"
#include "TimeUtil.h"
#include <chrono>

namespace
{
    uint64_t const nowNanoseconds()
    {
        auto const now = std::chrono::high_resolution_clock::now();
        auto const nowNano = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
        return nowNano.time_since_epoch().count();
    }
}

AbsoluteTouchMouse::AbsoluteTouchMouse(int deadzoneRadius, double doubleTapTime, VirtualDesktop desktopBounds)
    : deadzoneRadius{ deadzoneRadius },
    deadzoneRadiusSqr{ deadzoneRadius * deadzoneRadius },
    doubleTapNanoTime{ (uint32_t)timeutil::secondToNano(doubleTapTime) },
    doubleTapTimestamp{ 0 },
    moved{ false },
    mouse{},
    lastInitialFinger{},
    lastFingerCount{ 0 },
    desktop{desktopBounds}
{
    lastInitialFinger.id = UINT32_MAX;
    doubleTapTimestamp = nowNanoseconds();
}

void AbsoluteTouchMouse::Update(std::vector<VirtualFinger> const fingers)
{
    auto const now = nowNanoseconds();
    auto const delta = now - doubleTapTimestamp;
    if (fingers.size() != 0)
    {
        auto const& firstFinger = fingers[0];
        if (lastInitialFinger.id != UINT32_MAX && lastInitialFinger.id == firstFinger.id)
        {
            auto const lastPoint = Point<int64_t>{ lastInitialFinger.x, lastInitialFinger.y };
            auto const currentPoint = Point<int64_t>{ firstFinger.x, firstFinger.y };
            if (outsideDeadzoneSqr(deadzoneRadiusSqr, lastPoint, currentPoint))
            {
                if (delta <= doubleTapNanoTime)
                    mouse.Press(SupportedMouseButtons::Left);

                auto const virtualCoordinates = TransformCursorToVirtual(desktop, currentPoint);

                mouse.Move(SupportedMouseMovement::Absolute, (int)virtualCoordinates.x, (int)virtualCoordinates.y);
                lastInitialFinger = firstFinger;
                moved = true;
            }
        }
        else
        {
            lastInitialFinger = firstFinger;
        }

        lastFingerCount = fingers.size();
    }
}

void AbsoluteTouchMouse::Release()
{
    auto const now = nowNanoseconds();
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

void AbsoluteTouchMouse::Commit()
{
    mouse.Commit();
}

