// YAL zeldan

#ifndef CXM_LINEAR_HXX_
#define CXM_LINEAR_HXX_

#include "mystd.hxx"
#include "algebra.hxx"

CXM_NS_BEGIN

typedef unsigned int index_t;

template <typename H, int Q> class Vector;
template <typename H, int Q> class Covector;

template <typename K, unsigned int M, unsigned int N> class Matrix :
CXU_NS::Vectorial<Matrix<K, M, N>, K>
{
public:

    Matrix()
    {
        //CXU_LOOP(x, M) CXU_LOOP(y, N) _a[x][y] = K();
    }

    template <index_t P> Matrix<K, M, P> operator*(const Matrix<K, N, P> & a)
    {
        Matrix<K, M, P> m;
        CXU_LOOP(x, M)
        CXU_LOOP(y, P)
        CXU_LOOP(z, N)
        m(x, y) += (*this)(x, z) * a(z, y);
        return m;
    }

    const K & operator()(index_t x, index_t y) const
    {
        CXU_ASSERT(x < M);
        CXU_ASSERT(y < N);
        return _a[x][y];
    }

    K & operator()(index_t x, index_t y)
    {
        CXU_ASSERT(x < M);
        CXU_ASSERT(y < N);
        return _a[x][y];
    }

    void serialize(cxu::BOStorage & s) const
    {
        CXU_LOOP(x, M)
        CXU_LOOP(y, N)
        s << (*this)(x, y);
    }

    void serialize(cxu::BIStorage & s);
    void serialize(cxu::TOStorage & s) const;
    void serialize(cxu::TIStorage & s);

protected:

    Matrix<K, N, M> opposite() const
    {
        Matrix<K, N, M> m;
        CXU_LOOP(x, M)
        CXU_LOOP(y, N)
        m(x, y) = -(*this)(x, y);
        return m;
    }

    K _a[M][N];

};

template <typename K, index_t Q> class SquareMatrix :
public Matrix<K, Q, Q>,
public CXU_NS::Field<K>
{
public:
    K det() const;

};

template <typename H, int Q> class Vector :
public Matrix<H, Q, 1>
{
    using Matrix<H, Q, 1>::operator();

public:

    static void s_implement_Vector()
    {
        H::s_implement_Hield();
    }

    const H & operator()(index_t j) const
    {
        return (*this)(j, 0);
    }

    H & operator()(index_t j)
    {
        return (*this)(j, 0);
    }

    Covector<H, Q> co() const
    {
        Covector<H, Q> c;
        CXU_LOOP(j, Q)
        c(j) = (*this)(j).co();
        return c;
    }

    H qdr() const
    {
        return (co() * (*this))(0, 0);
    } //TODO meglio determinante di 1*1 ?

    H operator!() const
    {
        return qdr().sqrt();
    }

    H operator/(const Vector<H, Q> & u) const
    {
        return u.co() * (*this) / !u;
    }

    H operator|(const Vector<H, Q> & u) const
    {
        return u * ((*this) / u);
    }

    H operator%(const Vector<H, Q> & u) const
    {
        return (*this) - (*this | u);
    }

    H & operator|=(const Vector<H, Q> & u)
    {
        *this = *this | u;
        return *this;
    }

    H & operator%=(const Vector<H, Q> & u)
    {
        *this = *this % u;
        return *this;
    }


};

template <typename H, int Q> class Covector :
public Matrix<H, 1, Q>
{
    using Matrix<H, 1, Q>::operator();

public:

    const H & operator()(index_t j) const
    {
        return (*this)(0, j);
    }

    H & operator()(index_t j)
    {
        return (*this)(0, j);
    }

    Vector<H, Q> co() const
    {
        Vector<H, Q> v;
        CXU_LOOP(j, Q)
        v(j) = (*this)(j).co();
        return v;
    }

    H operator!() const
    {
        return ((*this) * co()).sqrt();
    }
};

template <typename H, index_t N> class Point :
public CXU_NS::Affine<Point<H, N>, Vector<H, N>, H>
{
protected:
    H _x[N];


public:

    Point();


    //!Compute \c this point transformated by \p t
    //const Point operator * (const Transform & t) const;

    //! Compound assignment
    /*! \sa operator * (const Transform & t) const */
    //Point & operator *= (const Transform & t);

    //! Compute projection of \c this point to line \p l
    //const Point operator | (const Line &) const;

    //! Compound assignment
    /*! \sa operator | (const Line & l) const */
    //Point & operator |= (const Line & l);

    void serialize(cxu::BOStorage &) const;
    void serialize(cxu::BIStorage &);
    void serialize(cxu::TOStorage &) const;
    void serialize(cxu::TIStorage &);

};

template <typename K> class Vector2 : public Vector<K, 2>
{
public:

    Vector2(const K & vx, const K & vy)
    {
        (*this)(0) = vx;
        (*this)(1) = vy;
    }

    const K & x() const
    {
        return (*this)(0);
    }

    const K & y() const
    {
        return (*this)(1);
    }

    const Vector2 ort() const
    {
        return Vector2(-y(), x());
    }

    const Vector2 cnj() const
    {
        return Vector2(x(), -y());
    }

    K operator ^ (const Vector2 & v) const
    {
        return x() * v.y() - y() * v.x();
    }

};

template <typename K> class Vector3 : public Vector<K, 3>
{
public:

    Vector3(const K & vx, const K & vy, const K & vz)
    {
        (*this)(0) = vx;
        (*this)(1) = vy;
        (*this)(2) = vz;
    }

    const K & x() const
    {
        return (*this)(0);
    }

    const K & y() const
    {
        return (*this)(1);
    }

    const K & z() const
    {
        return (*this)(2);
    }

    // TODO prodotto vettoriale
    //K operator ^ (const Vector2 & v) const {return x()*v.y() - y()*v.x();}

};




#if 0

template <typename R> class VectorR2 : public Vector2<R>
{
public:

    using Vector2<R>::x;
    using Vector2<R>::y;

    VectorR2(const R & x1, const R & x2) : Vector2<R>(x1, x2)
    {
    }

    R arg() const
    {
        return atan2(y(), x());
    }

    static const VectorR2 polar(const R& mag, const R& arg)
    {
        return VectorR2(mag * cos(arg), mag * sin(arg));
    }

};
#endif


CXM_NS_END

#endif /*LINEAR_HXX_*/
