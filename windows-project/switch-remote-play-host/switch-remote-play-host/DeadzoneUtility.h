#ifndef __DEADZONEUTILITY_H__
#define __DEADZONEUTILITY_H__

#include "Point.h"

#include <limits>

long long constexpr magnitudeSqr(long long x, long long y)
{
    return x * x + y * y;
}

template<typename numeral>
long long constexpr magnitudeSqr(Point<numeral> point)
{
    return magnitudeSqr(point.x, point.y);
}

bool outsideDeadzoneSqr(long long deadzoneRadiusSqr, long long x1, long long y1, long long x2, long long y2);

template<typename numeral>
bool outsideDeadzoneSqr(long long deadzoneRadiusSqr, Point<numeral> a, Point<numeral> b)
{
    return outsideDeadzoneSqr(deadzoneRadiusSqr, a.x, a.y, b.x, b.y);
}

#endif