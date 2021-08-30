#pragma once

template<typename numeral>
struct Point
{
    numeral x;
    numeral y;
};

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

bool outsideDeadzoneSqr(long long deadzoneRadiusSqr, long long x1, long long y1, long long x2, long long y2)
{
    auto dx = x1 - x2;
    auto dy = y1 - y2;
    auto magnitude = magnitudeSqr(dx, dy);

    return magnitude > deadzoneRadiusSqr;
}

template<typename numeral>
bool outsideDeadzoneSqr(long long deadzoneRadiusSqr, Point<numeral> a, Point<numeral> b)
{
    return outsideDeadzoneSqr(deadzoneRadiusSqr, a.x, a.y, b.x, b.y);
}