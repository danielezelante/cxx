// YAL zeldan

#ifndef CXM_REAL_HXX
#define CXM_REAL_HXX

#include <stdint.h>

#include <utility>

#if __sun__
#include <ieeefp.h>
#endif

#include <cxu/cxu.hxx>

#include "types.hxx"
#include "algebra.hxx"


CXM_NS_BEGIN


bool isPrime(unsigned int);
unsigned int nextPrime(unsigned int);

class Real :
public Field<Real>,
public cxu::TotalOrder<Real>,
public cxu::Set<Real, false>,
public cxu::XStringation<Real, char>,
public cxu::Serialization<Real>
{
public:

    Real() : _x(0)
    {
    }

    Real(double x) : _x(x)
    {
        if (!finite(_x))
        {
            CXU_ASSERT(false);
            CXU_THROWDATAEXCEPTION("Real out of domain");
        }
    }

    Real(const Real & a) = default;
    Real(Real && a) = default;

    static double s_tolerance;

    Real & operator=(const Real & a) = default;
    Real & operator=(Real && a) = default;

    Real co() const
    {
        return *this;
    }

    Real sqrt() const
    {
        return Real(::sqrt(value()));
    }

    Real exp() const
    {
        return Real(::exp(value()));
    }

    Real log() const
    {
        return Real(::log(value()));
    }

    Real log10() const
    {
        return Real(::log10(value()));
    }

    Real sin() const
    {
        return Real(::sin(value()));
    }

    Real cos() const
    {
        return Real(::cos(value()));
    }

    Real ceil() const
    {
        return Real(::ceil(value()));
    }

    static Real pow(const Real & x, const Real & y)
    {
        return Real(::pow(x.value(), y.value()));
    }

    double value() const
    {
        return _x;
    }

    // overridden to be more efficient

    Real operator!() const
    {
        return abs();
    }

    Real abs() const
    {
        return Real(::fabs(_x));
    }

    static Real minimum_positive()
    {
        return Real(std::numeric_limits<double>::min());
    }

    static Real epsilon()
    {
        return Real(std::numeric_limits<double>::epsilon());
    }

    static Real hypot(const Real & x, const Real & y)
    {
        return Real(::hypot(x.value(), y.value()));
    }

    bool isPositive() const
    {
        return _x > 0;
    }

    bool isNegative() const
    {
        return _x < 0;
    }

    void _Serialization_serialize(cxu::BOStorage & s) const
    {
        s << _x;
    }

    void _Serialization_serialize(cxu::BIStorage & s)
    {
        s >> _x;
    }

    void _Serialization_serialize(cxu::TOStorage & s) const
    {
        s << _x;
    }

    void _Serialization_serialize(cxu::TIStorage & s)
    {
        s >> _x;
    }

    std::string _XStringation_char_str() const
    {
        return cxu::Format("%$:{%$}") % cxu::ftod(_x) % cxu::ftox(_x);
    }

    std::string str_eng(int prec, bool plus = false) const
    {
        return cxu::realeng(value(), prec, plus);
    }

    int sgn() const
    {
        if (_x > 0) return +1;
        if (_x < 0) return -1;
        return 0;
    }

    // algebra interface

    static const Real & sdo_Monoid_null()
    {
        return s_1;
    }

    static Real sdo_Field_inverse(const Real & x)
    {
        return Real(1.0 / x.value());
    }

    static const Real & sdo_Abel_O()
    {
        return s_0;
    }

    static Real sdo_Abel_sum(const Real & x, const Real & y)
    {
        return Real(x.value() + y.value());
    }

    static Real sdo_Abel_opposite(const Real & x)
    {
        return Real(-x.value());
    }

    static Real sdo_Monoid_operation(const Real & x, const Real & y)
    {
        return Real(x.value() * y.value());
    }

    static int sdo_TotalOrder_compare(const Real & x, const Real & y)
    {
        if (::fabs(x.value() - y.value()) <= s_tolerance * ::fabs(x.value() + y.value())) return 0;
        if (x.value() < y.value()) return -1;
        if (x.value() > y.value()) return +1;
        return 0;
    }


protected:

    static const Real s_0;
    static const Real s_1;


private:
    double _x;


};



extern const double AUREA; // parte aurea
extern const double COAUREA;



// solves the 1st degree equation a*x+b=0
double solve1G(double a, double b);

// solves the 2nd degree equation a*x^2 + b*x + c = 0
// puts the lower solution in x1, and the higher in x2
// returns true id delta>=0
// if delta < 0 returns false, x1 and x2 are unchanged
std::pair<double, double> solve2G(double a, double b, double c);


double angle_0_2Pi(double alfa); /// porta l'angolo alfa nel range [0..2*Pi)
double angle_Pi_Pi(double alfa); /// porta l'anfolo alfa nel range [-Pi..Pi)

double stirling(double x); /// formula di Stirling per il fattoriale
double fibonacciGen(double x); /// numeri di Fibonacci, generati con formula


void solveC2G(const Complex & a, const Complex & b, const Complex & c, Complex & x1, Complex & x2);
// risolve in campo complesso un' equazione di secondo grado del tipo :
// a*x^2 + b*x + c = 0, le soluzioni sono messe in x1 e x2


CXM_NS_END

#endif

//.
