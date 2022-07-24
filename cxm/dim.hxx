// YAL zeldan

#ifndef CXM_DIM_HXX_
#define CXM_DIM_HXX_

#include <cxu/cxu.hxx>

#include "defs.hxx"
#include "vad.hxx"

#define CXM_TYPDIM <int D, int A, int M, int L, int T>
#define CXM_TYPDIM2 <int D0, int A0, int M0, int L0, int T0, int D1, int A1, int M1, int L1, int T1>
#define CXM_DEFDIM <D,A,M,L,T>
#define CXM_DEFDIM0 <D0,A0,M0,L0,T0>
#define CXM_DEFDIM1 <D1,A1,M1,L1,T1>



CXM_NS_BEGIN

        namespace dim
{

template CXM_TYPDIM class Dim : public cxu::Stringable
{
public:

    explicit Dim(const VaD & vv) : _v(vv)
    {
    }

    const VaD & v() const
    {
        return _v;
    }

    std::string str() const override
    {
        std::string mea;

        if (D)
        {
            if (A == 1) mea += "$ * ";
            else mea += cxu::Format("($^%$) * ") % A;
        }

        if (A)
        {
            if (A == 1) mea += "A * ";
            else mea += cxu::Format("(A^%$) * ") % A;
        }

        if (M)
        {
            if (M == 1) mea += "kg * ";
            else mea += cxu::Format("(g^%$) * ") % M;
        }

        if (L)
        {
            if (L == 1) mea += "m * ";
            else mea += cxu::Format("(m^%$) * ") % L;
        }

        if (T)
        {
            if (T == 1) mea += "s ";
            else mea += cxu::Format("(s^%$) * ") % T;
        }

        std::string rv = cxu::Format("%$ * %$") % _v.str() % mea;

        while (rv.length())
        {
            const size_t z = rv.length() - 1;
            if (rv[z] == ' ' || rv[z] == '*')
                rv.erase(z);
            else break;
        }

        return rv;
    }


protected:
    VaD _v;

};

template CXM_TYPDIM Dim CXM_DEFDIM operator+(const Dim CXM_DEFDIM & a, const Dim CXM_DEFDIM & b)
{
    return Dim CXM_DEFDIM(a.v() + b.v());
}

template CXM_TYPDIM Dim CXM_DEFDIM operator*(const VaD & k, const Dim CXM_DEFDIM & a)
{
    return Dim CXM_DEFDIM(k * a.v());
}

template CXM_TYPDIM Dim CXM_DEFDIM operator*(const Dim CXM_DEFDIM & a, const VaD & k)
{
    return Dim CXM_DEFDIM(a.v() * k);
}

template CXM_TYPDIM2 Dim<D0 + D1, A0 + A1, M0 + M1, L0 + L1, T0 + T1> operator*(const Dim CXM_DEFDIM0 & a, const Dim CXM_DEFDIM1 & b)
{
    return Dim < D0 + D1, A0 + A1, M0 + M1, L0 + L1, T0 + T1 > (a.v() * b.v());
}

template CXM_TYPDIM2 Dim<D0 - D1, A0 - A1, M0 - M1, L0 - L1, T0 - T1> operator/(const Dim CXM_DEFDIM0 & a, const Dim CXM_DEFDIM1 & b)
{
    return Dim < D0 - D1, A0 - A1, M0 - M1, L0 - L1, T0 - T1 > (a.v() / b.v());
}

typedef Dim<0, 0, 0, 0, 0> Number;
typedef Dim<1, 0, 0, 0, 0> Money;
typedef Dim<0, 1, 0, 0, 0> Current;
typedef Dim<0, 0, 1, 0, 0> Mass;
typedef Dim<0, 0, 0, 1, 0> Length;
typedef Dim<0, 0, 0, 0, 1> Time;

typedef Dim < 0, 0, 0, 1, -1 > Speed;
typedef Dim < 0, 0, 1, 1, -2 > Force;


extern const Mass kg;

extern const Length um;
extern const Length mm;
extern const Length cm;
extern const Length m;
extern const Length km;

extern const Time s;
extern const Time h;


template<> std::string Force::str() const;


VaD cos(const Number & a);



}
CXM_NS_END


#endif 
