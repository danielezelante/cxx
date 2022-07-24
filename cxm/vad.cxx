// YAL zeldan

#include <math.h>
#include <float.h>

#include "vad.hxx"

#include <cxu/mymacros.hxx>
#include "mymacros.hxx"

using namespace std;
using namespace cxu;

CXM_NS_BEGIN



static inline Real s_qdr(const Real & x)
{
    return x*x;
};

static inline VaD s_1arg(const VaD & x, const Real & f0, const Real & f1)
{
    return VaD(f0, s_qdr(f1) * x.qer());
}

static inline VaD s_2arg(const VaD & x, const VaD & y, const Real & f0, const Real & dx, const Real & dy)
{
    return VaD(f0, s_qdr(dx) * x.qer() + s_qdr(dy) * y.qer());
}





const Real VaD::s_stdError = Real::epsilon();

const VaD VaD::s_0 = VaD(Real::O(), Real::O());
const VaD VaD::s_1 = VaD(Real::I(), Real::O());

VaD VaD::sdo_Field_inverse(const VaD & a)
{
    const Real f0 = ~a.val();
    const Real f1 = -~s_qdr(a.val());

    return s_1arg(a, f0, f1);
}

VaD VaD::sdo_Abel_sum(const VaD & a, const VaD & b)
{
    return s_2arg(a, b, a.val() + b.val(), 1, 1);
}

VaD VaD::sdo_Monoid_operation(const VaD & a, const VaD & b)
{
    return s_2arg(a, b, a.val() * b.val(), b.val(), a.val());
}

const VaD pow(const VaD & vx, const VaD & vy)
{
    const Real x = vx.val();
    const Real y = vy.val();

    const Real f0 = Real::pow(x, y);
    const Real dx = Real::pow(x, y - Real::I()) * y;
    const Real dy = f0 * x.log();

    return s_2arg(vx, vy, f0, dx, dy);
}

const VaD pow(const VaD & vx, const Real & y)
{
    const Real x = vx.val();

    const Real f0 = Real::pow(x, y);
    const Real f1 = Real::pow(x, y - Real::I()) * y;

    return s_1arg(vx, f0, f1);
}

const VaD pow(const Real & x, const VaD & vy)
{
    const Real y = vy.val();

    const Real f0 = Real::pow(x, y);
    const Real f1 = f0 * x.log();

    return s_1arg(vy, f0, f1);
}

const VaD sqrt(const VaD & vx)
{
    const Real x = vx.val();

    const Real f0 = x.sqrt();
    const Real f1 = ~(f0 + f0);

    return s_1arg(vx, f0, f1);
}

const VaD exp(const VaD & vx)
{
    const Real x = vx.val();

    const Real f0 = x.exp();
    const Real f1 = f0;

    return s_1arg(vx, f0, f1);
}

const VaD log(const VaD & vx)
{
    const Real x = vx.val();

    const Real f0 = x.log();
    const Real f1 = ~x;

    return s_1arg(vx, f0, f1);
}

const VaD cos(const VaD & vx)
{
    const Real x = vx.val();

    const Real f0 = x.cos();
    const Real f1 = -x.sin();

    const VaD y = s_1arg(vx, f0, f1);
    return y;
}

const VaD sin(const VaD & vx)
{
    const Real x = vx.val();

    const Real f0 = x.sin();
    const Real f1 = x.cos();

    const VaD y = s_1arg(vx, f0, f1);
    return y;
}

void VaD::_Serialization_serialize(BOStorage & s) const
{
    s << _v << _q;
}

void VaD::_Serialization_serialize(BIStorage & s)
{
    s >> _v >> _q;
}

void VaD::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _v << "," << _q << ")";
}

void VaD::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _v >> "," >> _q >> ")";

    if (_q.isNegative()) _q = -_q;
    if (_q < Real::minimum_positive()) _q = Real::minimum_positive();
}

std::string VaD::_XStringation_char_str() const
{
    const Real d = _q.sqrt();
    const int gd =
            d > Real::minimum_positive() && _v.abs() > Real::minimum_positive() ?
            std::max(3, static_cast<int> ((_v / d).abs().log10().ceil().value()))
            : 3;

    return Format("(%$~%$)")
            % _v.str_eng(gd)
            % d.str_eng(3)
            ;
}



CXM_NS_END
