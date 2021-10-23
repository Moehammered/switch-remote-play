#ifndef __INTERPOLATION_H__
#define  __INTERPOLATION_H__

template<typename Real>
Real lerp(Real from, Real to, double ratio)
{
    auto const range = to - from;
    return from + range * ratio;
}

template<typename Real>
Real normalise(Real from, Real to, Real value)
{
    auto const range = to - from;
    auto const delta = value - from;

    return delta / range;
}

#endif
