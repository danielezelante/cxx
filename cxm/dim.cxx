// YAL zeldan

#include "mymacros.hxx"


#include "dim.hxx"


CXM_NS_BEGIN

        namespace dim
{
const Mass kg = Mass(VaD(1E+0, 0));

const Length um = Length(VaD(1E-6, 0));
const Length mm = Length(VaD(1E-3, 0));
const Length cm = Length(VaD(1E-2, 0));
const Length m = Length(VaD(1, 0));
const Length km = Length(VaD(1E+3, 0));

const Time s = Time(VaD(1, 0));
const Time h = Time(VaD(3600, 0));

const Force N = Force(VaD(1, 0));

const Money dollar = Money(VaD(1, 0));

template<> std::string Force::str() const
{
    return cxu::Format("%$ * N") % _v;
}

VaD cos(const Number & a)
{
    return cos(a.v());
}

}


CXM_NS_END
