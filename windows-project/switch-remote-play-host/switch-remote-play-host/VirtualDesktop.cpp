#include "VirtualDesktop.h"
#include "Interpolation.h"
#include <algorithm>

VirtualDesktop MapVirtualDesktop(std::vector<DisplayDeviceInfo> const displays, int64_t const virtualEndX, int64_t const virtualEndY)
{
    auto minPoint = Point<int64_t>{ INT64_MAX, INT64_MAX };
    auto maxPoint = Point<int64_t>{ INT64_MIN, INT64_MIN };

    for (auto const& display : displays)
    {
        minPoint.x = std::min(display.x, minPoint.x);
        minPoint.y = std::min(display.y, minPoint.y);

        maxPoint.x = std::max(display.x + display.width, maxPoint.x);
        maxPoint.y = std::max(display.y + display.height, maxPoint.y);
    }

    return { minPoint, maxPoint, { virtualEndX, virtualEndY } };
}

Point<double> NormaliseCursor(VirtualDesktop const& desktop, Point<int64_t> const absolutePosition)
{
    auto const x = normalise<double>(desktop.start.x, desktop.end.x, absolutePosition.x);
    auto const y = normalise<double>(desktop.start.y, desktop.end.y, absolutePosition.y);
    return { x,y };
}

Point<int64_t> TransformCursorToVirtual(VirtualDesktop const& desktop, Point<int64_t> const absolutePosition)
{
    auto const normalisedCursor = NormaliseCursor(desktop, absolutePosition);

    return Point<int64_t>{
        (int64_t)(normalisedCursor.x * desktop.virtualEnd.x),
        (int64_t)(normalisedCursor.y * desktop.virtualEnd.y)
    };
}