#include "DeadzoneUtility.h"

bool outsideDeadzoneSqr(long long deadzoneRadiusSqr, long long x1, long long y1, long long x2, long long y2)
{
    auto dx = x1 - x2;
    auto dy = y1 - y2;
    auto magnitude = magnitudeSqr(dx, dy);

    return magnitude > deadzoneRadiusSqr;
}