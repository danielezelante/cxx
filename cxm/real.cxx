// YAL zeldan


#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>

#include <cxu/cxu.hxx>

#include "mymacros.hxx"
#include "real.hxx"

using namespace cxu;

CXM_NS_BEGIN

        double Real::s_tolerance = std::numeric_limits<double>::epsilon();

const Real Real::s_0 = Real(0.0);
const Real Real::s_1 = Real(1.0);

const double AUREA = ((::sqrt(5.0) - 1.0) / 2.0);
const double COAUREA = ((-::sqrt(5.0) - 1.0) / 2.0);

double solve1G(const double a, const double b)
{
    return -b / a;
}

std::pair<double, double> solve2G(const double a, const double b, const double c)
{
    const double delta = sqr(b) - 4 * a*c;
    if (delta < 0)
        return std::make_pair(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()); //only for optimization
    const double sqdelta = ::sqrt(delta);
    return std::make_pair((-b + sqdelta) / (2 * a), (-b - sqdelta) / (2 * a));
}

double angle_0_2Pi(const double a)
{
    double x = ::fmod(a, 2 * M_PI);
    if (x < 0) x += (2 * M_PI);
    return x;
}

double angle_Pi_Pi(const double a)
{
    double x = ::fmod(a, 2 * M_PI);
    if (x > M_PI) x -= (2 * M_PI);
    if (x<-M_PI) x += (2 * M_PI);
    return x;
}

double stirling(double x)
{
    return ::pow(x, x) * ::exp(-x) * ::sqrt(2 * M_PI * x);
}

double fibonacciGen(double x)
{
    return 1 / (::sqrt(5.0)*::pow(AUREA, x)) - 1 / (::sqrt(5.0)*::pow(COAUREA, x));
}

void solveC2G(const Complex & a, const Complex & b, const Complex & c, Complex & x1, Complex & x2)
{
    const Complex qdelta = std::sqrt(b * b - a * c * 4.0);
    x1 = (-b + qdelta) / 2.0;
    x2 = (-b - qdelta) / 2.0;
}




CXM_NS_END

//.
