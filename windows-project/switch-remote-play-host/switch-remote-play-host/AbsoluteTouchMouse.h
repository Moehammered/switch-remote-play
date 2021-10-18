#ifndef __ABSOLUTETOUCHMOUSE_H__
#define __ABSOLUTETOUCHMOUSE_H__

#include "VirtualFinger.h"
#include "VirtualMouse.h"
#include "Point.h"
#include <stdint.h>
#include <vector>

class AbsoluteTouchMouse
{
public:
    AbsoluteTouchMouse(int deadzoneRadius, double doubleTapTime,
        int64_t sessionWidth, int64_t sessionHeight,
        Point<int32_t> topLeft, Point<int32_t> bottomRight);

    void Update(std::vector<VirtualFinger> const fingers);
    void Release();

    void Commit();

private:
    int const deadzoneRadius;
    int const deadzoneRadiusSqr;
    uint32_t const doubleTapNanoTime;
    uint64_t doubleTapTimestamp;
    bool moved;

    VirtualMouse mouse;
    VirtualFinger lastInitialFinger;
    size_t lastFingerCount;

    int64_t const sessionWidth;
    int64_t const sessionHeight;
    Point<int32_t> const topLeft;
    Point<int32_t> const bottomRight;
};

#endif
