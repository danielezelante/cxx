// YAL zeldan

#ifndef CXM_VAD_HXX
#define CXM_VAD_HXX

#include <cxu/cxu.hxx>

#include "defs.hxx"
#include "real.hxx"

CXM_NS_BEGIN


class VaD :
public cxu::Serialization<VaD>,
public cxu::XStringation<VaD, char>,
public Field<VaD>,
public cxu::TotalOrder<VaD>,
public cxu::Set<VaD, false>
{
    CXU_DECLARE_SERIALIZABLE(VaD)

    friend const VaD pow(const VaD & a, const VaD & b);
    friend const VaD pow(const VaD & a, const CXM_NS::Real & b);
    friend const VaD pow(const CXM_NS::Real & a, const VaD & b);
    friend const VaD sqrt(const VaD & a);

    friend const VaD exp(const VaD & a);
    friend const VaD log(const VaD & a);

    friend const VaD cos(const VaD & a);
    friend const VaD sin(const VaD & a);


protected:
    CXM_NS::Real _v;
    CXM_NS::Real _q;


public:

    const CXM_NS::Real & val() const
    {
        return _v;
    }

    const CXM_NS::Real & qer() const
    {
        return _q;
    }

    VaD()
    : _v(0.0)
    , _q(CXM_NS::Real::minimum_positive())
    {
    }

    explicit VaD(const CXM_NS::Real & v)
    : _v(v)
    , _q(CXM_NS::Real(std::max(CXM_NS::Real::minimum_positive(), v * v * s_stdError)))
    {
    }

    // quadratic error

    VaD(const CXM_NS::Real & v, const CXM_NS::Real & q)
    : _v(v)
    , _q(q)
    {
    }

    // linear error

    static VaD vd(const CXM_NS::Real & v, const CXM_NS::Real & d)
    {
        return VaD(v, d * d);
    }


    //using CXM_NS::algebra::Hield<VaD>::operator *;
    //using CXM_NS::algebra::Hield<VaD>::operator /;

    VaD operator*(const CXM_NS::Real & k)
    {
        return VaD(val() * k, qer() * k * k);
    }

    VaD operator/(const CXM_NS::Real & k)
    {
        return (*this) * (~k);
    }

    std::string _XStringation_char_str() const;

    static const CXM_NS::Real s_stdError;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);


    // algebra interface

    static const VaD & sdo_Abel_O()
    {
        return s_0;
    }

    static VaD sdo_Abel_opposite(const VaD & x)
    {
        return VaD(-x.val(), x.qer());
    }

    static VaD sdo_Field_inverse(const VaD &);
    static VaD sdo_Abel_sum(const VaD &, const VaD &);
    static VaD sdo_Monoid_operation(const VaD &, const VaD &);

    static const VaD & sdo_Monoid_null()
    {
        return s_1;
    }

    static const VaD & alegbra_Field_conj(const VaD & x)
    {
        return x;
    }

    static int sdo_TotalOrder_compare(const VaD & x, const VaD & y)
    {
        const VaD d = x - y;
        if (d.qer() > d.val() * d.val()) return 0;
        return d.val().sgn();
    }


protected:

    static const VaD s_0;
    static const VaD s_1;

};


const VaD pow(const VaD & a, const VaD & b);
const VaD pow(const VaD & a, const CXM_NS::Real & b);
const VaD pow(const CXM_NS::Real & a, const VaD & b);
const VaD sqrt(const VaD & a);

const VaD exp(const VaD & a);
const VaD log(const VaD & a);
const VaD cos(const VaD & a);


CXM_NS_END

#endif
