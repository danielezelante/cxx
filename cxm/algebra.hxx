// YAL zeldan

#ifndef CXM_ALGEBRA_HXX_
#define CXM_ALGEBRA_HXX_

#include <cxu/cxu.hxx>

CXM_NS_BEGIN
template <typename T> class Field; // commutative Ring with inverse for all elements except zero
template <typename V, typename K> class Vectorial;
template <typename P, typename V, typename K> class Affine;

template <typename T> class Field : public CXU_NS::Ring<T, true>
{
    //CXU_CRTP_DEFINES(Field, T)

public:

    static T conj(const Field<T> & a)
    {
        return T::sdo_Field_conj(a);
    }

};

template <typename T> T operator~(const Field<T> & a)
{
    return T::sdo_Field_inverse(CXU_CRTP_CV(T, a));
}

template <typename T> T operator/(const Field<T> & a, const Field<T> & b)
{
    return a * ~b;
}

template <typename T> T & operator/=(Field<T> & a, const Field<T> & b)
{
    a = a / b;
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

template <typename V, typename K> class Vectorial : public CXU_NS::Abel<V>
{
    //CXU_CRTP_DEFINES(Vectorial, V, K)

public:

};

template <typename V, typename K> V operator*(const Vectorial<V, K> & v, const Field<K> & k)
{
    CXU_TRACE("Vectorial *");
#ifdef cxu_DEBUG
    const V & v1 = CXU_CRTP_CV(V, v);
    const V & i1 = V::sdo_Vectorial_linear(v1, CXU_NS::Ring<K, true>::I());
    CXU_ASSERT(v1 == i1);
#endif
    return V::sdo_Vectorial_linear(CXU_CRTP_CV(V, v), CXU_CRTP_CV(K, k));
}

template <typename V, typename K> V operator/(const Vectorial<V, K> & v, const Field<K> & k)
{
    return v * ~k;
}

template <typename P, typename V, typename K> class Affine
{
    //CXU_CRTP_DEFINES(Affine, P, V, K)

public:


};

template <typename P, typename V, typename K> P operator+(const Affine<P, V, K> & p, const Vectorial<V, K> & v)
{
    return P::sdo_Point_apply(CXU_CRTP_CV(P, p), CXU_CRTP_CV(V, v));
}

template <typename P, typename V, typename K> P & operator+=(Affine<P, V, K> & p, const Vectorial<V, K> & v)
{
    p = p + v;
    return p;
}

template <typename P, typename V, typename K> P operator-(const Affine<P, V, K> & p, const Vectorial<V, K> & v)
{
    return p + (-v);
}

template <typename P, typename V, typename K> P & operator-=(Affine<P, V, K> & p, const Vectorial<V, K> & v)
{
    p = p - v;
    return p;
}

template <typename P, typename V, typename K> V operator-(const Affine<P, V, K> & p0, const Affine<P, V, K> & p1)
{ // operator - notation is opposite to unapply notation
    const V v = P::sdo_Point_unapply(CXU_CRTP_CV(P, p1), CXU_CRTP_CV(P, p0));
    return v;
}





CXM_NS_END

#endif /*ALGEBRA_HXX_*/

//.
