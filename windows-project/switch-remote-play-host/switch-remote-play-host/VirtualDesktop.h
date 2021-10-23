#ifndef __VIRTUALDESKTOP_H__
#define __VIRTUALDESKTOP_H__

#include <vector>
#include "DisplayDeviceService.h"
#include "Point.h"

struct VirtualDesktop
{
    Point<int64_t> start;
    Point<int64_t> end;
    Point<int64_t> virtualEnd;
};

VirtualDesktop MapVirtualDesktop(std::vector<DisplayDeviceInfo> const displays, int64_t const virtualEndX, int64_t const virtualEndY);
Point<double> NormaliseCursor(VirtualDesktop const& desktop, Point<int64_t> const absolutePosition);
Point<int64_t> TransformCursorToVirtual(VirtualDesktop const& desktop, Point<int64_t> const absolutePosition);

#endif
