#ifndef __ABSOLUTETOUCHMOUSE_H__
#define __ABSOLUTETOUCHMOUSE_H__

#include "TouchInterface.h"
#include "VirtualFinger.h"
#include "VirtualMouse.h"
#include "Point.h"
#include "VirtualDesktop.h"
#include <stdint.h>
#include <vector>

class AbsoluteTouchMouse : public TouchInterface
{
public:
    AbsoluteTouchMouse(int deadzoneRadius, double doubleTapTime,
                        VirtualDesktop desktopBounds);

    void Update(std::vector<VirtualFinger> const fingers) override;
    void Release() override;

    void Commit() override;

private:
    int const deadzoneRadius;
    int const deadzoneRadiusSqr;
    uint32_t const doubleTapNanoTime;
    uint64_t doubleTapTimestamp;
    bool moved;

    VirtualMouse mouse;
    VirtualFinger lastInitialFinger;
    size_t lastFingerCount;

    VirtualDesktop const desktop;
};

#endif
