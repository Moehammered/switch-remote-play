#ifndef __TIMEUTIL_H__
#define __TIMEUTIL_H__

#include <stdint.h>
#include <cmath>

namespace timeutil
{
    double constexpr secondToNanoRatio = 1000 * 1000 * 1000;

    double constexpr nanoToSecond(uint64_t const nano)
    {
        return nano / secondToNanoRatio;
    }

    uint64_t constexpr secondToNano(double const seconds)
    {
        auto nanoseconds = seconds * secondToNanoRatio;
        return (uint64_t)nanoseconds;
    }
}

#endif