#ifndef __POINT_H__
#define __POINT_H__

template<typename numeral>
struct Point
{
    numeral x;
    numeral y;
};

template<typename Real>
Point<Real> operator+(Point<Real> const a, Point<Real> const b)
{
    return Point<Real>{a.x + b.x, a.y + b.y};
}

template<typename Real>
Point<Real> operator-(Point<Real> const a, Point<Real> const b)
{
    return Point<Real>{a.x - b.x, a.y - b.y};
}

template<typename Real>
Point<Real> operator*(Point<Real> const a, Real const scalar)
{
    return Point<Real>{a.x * scalar, a.y * scalar};
}

template<typename Real>
Point<Real> operator*(Real const scalar, Point<Real> const a)
{
    return Point<Real>{a.x * scalar, a.y * scalar};
}

template<typename Real>
Point<Real> operator/(Point<Real> const a, Real const scalar)
{
    return Point<Real>{a.x / scalar, a.y / scalar};
}

template<typename Real>
Point<double> operator/(Point<Real> const a, double const scalar)
{
    return Point<double>{a.x / scalar, a.y / scalar};
}

#endif
