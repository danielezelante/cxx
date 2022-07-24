// YAL zeldan

#ifndef CXU_ALGEBRA_HXX_
#define CXU_ALGEBRA_HXX_

#include "defs.hxx"
#include "globals.hxx"
#include "object.hxx"

CXU_NS_BEGIN

#define UU __attribute__ ((unused))

template <typename T> class Check
{
public:

    static void check()
    {
        T::sdo_Check();
    }

    static void check(const T & x)
    {
        T::sdo_Check();

        T::sdo_Check(x);
    }

    static void check(const T & x, const T & y)
    {
        T::sdo_Check();

        T::sdo_Check(x);
        T::sdo_Check(y);

        T::sdo_Check(x, x);
        T::sdo_Check(x, y);
        T::sdo_Check(y, x);
        T::sdo_Check(y, y);
    }

    static void check(const T & x, const T & y, const T & z)
    {
        T::sdo_Check();

        T::sdo_Check(x);
        T::sdo_Check(y);
        T::sdo_Check(z);

        T::sdo_Check(x, x);
        T::sdo_Check(x, y);
        T::sdo_Check(x, z);
        T::sdo_Check(y, x);
        T::sdo_Check(y, y);
        T::sdo_Check(y, z);
        T::sdo_Check(z, x);
        T::sdo_Check(z, y);
        T::sdo_Check(z, z);

        T::sdo_Check(x, x, x);
        T::sdo_Check(x, x, y);
        T::sdo_Check(x, x, z);
        T::sdo_Check(x, y, x);
        T::sdo_Check(x, y, y);
        T::sdo_Check(x, y, z);
        T::sdo_Check(x, z, x);
        T::sdo_Check(x, z, y);
        T::sdo_Check(x, z, z);
        T::sdo_Check(y, x, x);
        T::sdo_Check(y, x, y);
        T::sdo_Check(y, x, z);
        T::sdo_Check(y, y, x);
        T::sdo_Check(y, y, y);
        T::sdo_Check(y, y, z);
        T::sdo_Check(y, z, x);
        T::sdo_Check(y, z, y);
        T::sdo_Check(y, z, z);
        T::sdo_Check(z, x, x);
        T::sdo_Check(z, x, y);
        T::sdo_Check(z, x, z);
        T::sdo_Check(z, y, x);
        T::sdo_Check(z, y, y);
        T::sdo_Check(z, y, z);
        T::sdo_Check(z, z, x);
        T::sdo_Check(z, z, y);
        T::sdo_Check(z, z, z);

    }
};



template <typename T, bool exact> class Set;
template <typename T> class Monoid;
template <typename T> class Group;
template <typename T> class Abel;
template <typename T, bool commuta> class Ring;
template <typename T> class Order;
template <typename T> class TotalOrder;
template <typename P, typename V> class Operabel;





// Set

template <typename T, bool exact> class Set : public Check< Set<T, exact> >
{
    //CXU_CRTP_DEFINES(Set, T, exact);

public:

    static void sdo_Check()
    {
    }

    static void sdo_Check(const Set<T, exact> & a UU)
    {
        CXU_ASSERT(a == a);
    }

    static void sdo_Check(const Set<T, exact> & a UU, const Set<T, exact> & b UU)
    {
        CXU_ASSERT((a == b) == (b == a));
    }

    static void sdo_Check(const Set<T, exact> & a, const Set<T, exact> & b, const Set<T, exact> & c)
    {
        unsigned n = 0;
        if (a == b) ++n;
        if (b == c) ++n;
        if (c == a) ++n;
        CXU_ASSERT(n != 2 || !exact);
    }
};

template <typename T, bool exact> bool operator==(const Set<T, exact> & a, const Set<T, exact> & b)
{
    return T::sdo_Set_equals(CXU_CRTP_CV(T, a), CXU_CRTP_CV(T, b));
}

template <typename T, bool exact> bool operator!=(const Set<T, exact> & a, const Set<T, exact> & b)
{
    return !(a == b);
}




// Monoid

template <typename T> class Monoid : public Check<Monoid<T> >
{
    //CXU_CRTP_DEFINES(Monoid, T)

public:

    static const T & e()
    {
        return T::sdo_Monoid_null();
    }

    static void sdo_Check()
    {
        CXU_ASSERT(e() == e());
        CXU_ASSERT(e() * e() == e());
    }

    static void sdo_Check(const Monoid<T> & a UU)
    {
        CXU_ASSERT(a * e() == a);
        CXU_ASSERT(e() * a == a);
    }

    static void sdo_Check(const Monoid<T> & a UU, const Monoid<T> & b UU, const Monoid<T> & c UU)
    {
        CXU_ASSERT(a * (b * c) == (a * b) * c);
    }
};

template <typename T> T operator*(const Monoid<T> & a, const Monoid<T> & b)
{
    return T::sdo_Monoid_operation(CXU_CRTP_CV(T, a), CXU_CRTP_CV(T, b));
}

template <typename T> T & operator*=(Monoid<T> & a, const Monoid<T> & b)
{
    a = a * b;
    return a;
}

template <typename T> class Group : public Monoid<T>, public Check<Group<T> >
{
    //CXU_CRTP_DEFINES(Group, T)

public:

    using Monoid<T>::e;

    static void sdo_Check()
    {
        Monoid<T>::sdo_Check();
        CXU_ASSERT(~e() == e());
    }

    void sdo_Check(const Group<T> & a)
    {
        Monoid<T>::sdo_Check(a);
        CXU_ASSERT(a*~a == e());
        CXU_ASSERT(~a * a == e());
    }

};

template <typename T> Group<T> operator~(const Group<T> & x)
{
    return T::sdo_Group_inverse(x);
}

template <typename T> class Abel : public Check<Abel<T> >
{
    //CXU_CRTP_DEFINES(Abel, T)

public:

    static const T & O()
    {
        return T::sdo_Abel_O();
    }

    static void sdo_Check()
    {
        CXU_ASSERT(O() == O());
        CXU_ASSERT(O() + O() == O());
    }

    static void sdo_Check(const Abel<T> & a UU)
    {
        CXU_ASSERT(a + O() == a);
        CXU_ASSERT(O() + a == a);
        CXU_ASSERT(a + -a == O());
        CXU_ASSERT(-a + a == O());
    }

    static void sdo_Check(const Abel<T> & a UU, const Abel<T> & b UU)
    {
        CXU_ASSERT(a + b == b + a);
    }

    static void sdo_Check(const Abel<T> & a UU, const Abel<T> & b UU, const Abel<T> & c UU)
    {
        CXU_ASSERT(a + (b + c) == (a + b) + c);
    }

};

template <typename T> T operator+(const Abel<T> & a, const Abel<T> & b)
{
    return T::sdo_Abel_sum(CXU_CRTP_CV(T, a), CXU_CRTP_CV(T, b));
}

template <typename T> T & operator+=(Abel<T> & a, const Abel<T> & b)
{
    T & ta = CXU_CRTP_OV(T, a);
    ta = a + b;
    return ta;
}

template <typename T> T operator-(const Abel<T> & a)
{
    return T::sdo_Abel_opposite(CXU_CRTP_CV(T, a));
}

template <typename T> T operator-(const Abel<T> & a, const Abel<T> & b)
{
    return a + (-b);
}

template <typename T> Abel<T> & operator-=(Abel<T> & a, const Abel<T> & b)
{
    a = a - b;
    return a;
}



template <typename T, bool commuta> class Ring : public Abel<T>, public Monoid<T>, public Check<Ring<T, commuta>>
{
    //CXU_CRTP_DEFINES(Ring, T, commuta)

    public:

    static const T & I()
    {
        return Monoid<T>::e();
    }


    // TODO check_3 how to perform distributive check on sum and product ?

};

template <class T, bool commuta> T operator+(const Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    return static_cast<const Abel<T> &> (a) + static_cast<const Abel<T> &> (b);
}

template <class T, bool commuta> T operator-(const Ring<T, commuta> & a)
{
    return -static_cast<const Abel<T> &> (a);
}

template <class T, bool commuta> T operator-(const Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    return a + (-b);
}

template <class T, bool commuta> T & operator+=(Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    a = a + b;
    return a;
}

template <class T, bool commuta> T & operator-=(Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    a = a - b;
    return a;
}

template <class T, bool commuta> T operator*(const Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    return static_cast<const Monoid<T> &> (a) * static_cast<const Monoid<T> &> (b);
}

template <class T, bool commuta> T & operator*=(Ring<T, commuta> & a, const Ring<T, commuta> & b)
{
    a = a * b;
    return a;
}

template <typename T> class Order
{
    //CXU_CRTP_DEFINES(Order, T)

public:

    static int compare(const Order<T> & a, const Order<T> & b)
    {
        return T::sdo_Order_compare(CXU_CRTP_CV(T, a), CXU_CRTP_CV(T, b));
    }
};

template <typename T> bool operator<(const Order<T> & a, const Order<T> & b)
{
    return Order<T>::compare(a, b) < 0;
}

template <typename T> bool operator>(const Order<T> & a, const Order<T> & b)
{
    return Order<T>::compare(a, b) > 0;
}

template <typename T> class TotalOrder : public Order<T>
{
    //CXU_CRTP_DEFINES(TotalOrder, T)

public:

    static int sdo_Order_compare(const T & a, const T & b)
    {
        const int x = T::sdo_TotalOrder_compare(a, b);
        if (!x) CXU_ASSERT(a == b);
        return x;
    }

    // fornisce un Set potenziale

    static bool sdo_Set_equals(const T & a, const T & b)
    {
        return !T::sdo_TotalOrder_compare(a, b);
    }
};

template <typename T> bool operator<=(const TotalOrder<T> & a, const TotalOrder<T> & b)
{
    return TotalOrder<T>::compare(a, b) <= 0;
}

template <typename T> bool operator>=(const TotalOrder<T> & a, const TotalOrder<T> & b)
{
    return TotalOrder<T>::compare(a, b) >= 0;
}

template <typename P, typename V> class Operabel
{
public:


};

template <typename P, typename V> P operator+(const Operabel<P, V> & p, const Abel<V> & v)
{
    return P::sdo_Point_apply(CXU_CRTP_CV(P, p), CXU_CRTP_CV(V, v));
}

template <typename P, typename V> P & operator+=(Operabel<P, V> & p, const Abel<V> & v)
{
    p = p + v;
    return p;
}

template <typename P, typename V> P operator-(const Operabel<P, V> & p, const Abel<V> & v)
{
    return p + (-v);
}

template <typename P, typename V> P & operator-=(Operabel<P, V> & p, const Abel<V> & v)
{
    p = p - v;
    return p;
}

template <typename P, typename V> V operator-(const Operabel<P, V> & p0, const Operabel<P, V> & p1)
{ // operator - notation is opposite to unapply notation
    const V v = P::sdo_Point_unapply(CXU_CRTP_CV(P, p1), CXU_CRTP_CV(P, p0));
    return v;
}



#undef UU

CXU_NS_END

#endif /*ALGEBRA_HXX_*/

//.
