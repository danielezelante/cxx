// YAL zeldan

#ifndef CXM_LINEAR2D_HXX
#define CXM_LINEAR2D_HXX

#include <set>

#if __sun__
#include <ieeefp.h>
#endif

#include <cxu/cxu.hxx>


#include "defs.hxx"
#include "types.hxx"

CXM_NS_BEGIN

//! Vector in \f$\Re^2\f$
/*! Vector is stored using its cartesian components namely \a x and \a y .
 *  \n This class in nearly-immutable: only serialize, assignment and compound assignment can change its state.
 *  \n Hereafter in this page:
 *  - v is used for vector
 *  - u is used for versor
 *  - k is used for scalar
 *  .
 *  - subscript 0 is used for \c this vector
 *  - subscript 1,2..n are used for parameters and for binary operators
 *  - subscript r is used for return value
 *  .
 */
class Vector2D : public cxu::Serialization<Vector2D>
{
    CXU_DECLARE_SERIALIZABLE(Vector2D)

private:
    double _x, _y;


public:

    //! Default constructor

    /*! Construct vector setting \a x and \a y components to 0 */
    Vector2D() : _x(0), _y(0)
    {
    }

    //! User constructor

    /*! Construct vector setting \a x and \a y components as specified in \p px and \p py */
    Vector2D(double px, double py) : _x(px), _y(py)
    {
    }

    /*! \return \a x component*/
    double x() const
    {
        return _x;
    }

    /*! \return \a y component*/
    double y() const
    {
        return _y;
    }

    //! Construct from \c Point2D
    /*! Constructed vector is <tt> (p - Point2D::O) </tt> */
    explicit Vector2D(const Point2D & p);

    //! Construct from \c Size2D
    /*! Constructed vector has same components as \p a */
    explicit Vector2D(const Size2D & a);

    //! Construct from \c Versor2D
    /*! Constructed vector has same components as \p u */
    explicit Vector2D(const Versor2D & u);

    //! Module

    /*! Compute module (or norma, or length) of this vector
     *  \return \f$\|v_0\|=\sqrt{({{v_0}_x}^2+{{v_0}_y}^2)}\f$
     */
    double operator!() const
    {
        return ::hypot(_y, _x);
    }

    //! Argument

    /*! Compute argument of this vector (angle from X positive semiaxis and this vector)
     *	\return Angle in range \f$[-\pi;+\pi)\f$
     */
    double operator~() const
    {
        return atan2(_y, _x);
    }

    //! Unary plus (NOP)

    /*! \note Unary plus has no effect, since <tt>+v == v</tt> */
    const Vector2D & operator+() const
    {
        return *this;
    }

    //! Opposite

    /*! \return Vector having opposite components */
    const Vector2D operator-() const
    {
        return Vector2D(-_x, -_y);
    }

    //! Null
    /*! Vector with both components set to \c 0 */
    static const Vector2D O;

    //! Polar pseudo-constructor

    /*! \return Vector having: modulus = \p mag, argument = \p arg
     *  \post <tt>double mag,arg; v = polar(mag,arg);</tt>
     *  \n
     *  \n <tt> !v == mag; </tt>
     *	\n <tt> ~v == \f$(arg \bmod 2\pi)-\pi\f$;</tt> ( in range \f$[-\pi;+\pi)\f$ )
     */
    static const Vector2D polar(double mag, double arg)
    {
        return Vector2D(mag * cos(arg), mag * sin(arg));
    }

    //! Field product

    /*  Both components are multiplied by \c k */
    const Vector2D operator*(double k) const
    {
        return Vector2D(_x*k, _y * k);
    }

    //! Field division

    /*! \pre \f$k\ne 0\f$
     *	\return \f$v_0/k=v_0\cdot(1/k)\f$
     */
    const Vector2D operator/(double k) const
    {
        return Vector2D(_x / k, _y / k);
    }

    //! Orthogonal

    /*! \return Vector \f$\pi/2\f$ counterclokwise rotated. */
    const Vector2D ort() const
    {
        return Vector2D(-_y, _x);
    }

    //! Conjugate

    /*! \return Vector having same \a x component and opposite \a y component.*/
    const Vector2D cnj() const
    {
        return Vector2D(_x, -_y);
    }

    //! Squared modulus

    /*! \return \f$({{v_0}_x}^2+{{v_0}_y}^2)\f$
     *  \note This function is faster then <tt> operator !</tt> because it does not use \c sqrt
     */
    double qdr() const
    {
        return (_x * _x + _y * _y);
    }

    //! Compound + assignment
    /*! \sa operator + (const Vector2D & v) const */
    Vector2D & operator+=(const Vector2D & v);

    //! Compound - assignment
    /*! \sa operator - (const Vector2D & v) const */
    Vector2D & operator-=(const Vector2D & v);

    //! Sum

    /*! \return \f$v_r \models v_1+v_2\f$
     *  \n \f${v_r}_x = {v_1}_x + {v_2}_x\f$
     *  \n \f${v_r}_y = {v_1}_y + {v_2}_y\f$
     */
    const Vector2D operator+(const Vector2D & v) const
    {
        return Vector2D(_x + v.x(), _y + v.y());
    }

    //! Difference

    /*! \sa operator + (const Vector2D &) const
     *  \sa operator - () const
     *  \return \f$v_1-v_2=v_1+(-v_2)\f$
     */
    const Vector2D operator-(const Vector2D & v) const
    {
        return Vector2D(_x - v.x(), _y - v.y());
    }

    //! Scalar (or dot) product

    /*! \return \f$ v_1\cdot v_2 = {v_1}_x {v_2}_x + {v_1}_y {v_2}_y \f$ */
    double operator*(const Vector2D & v) const
    {
        return _x * v._x + _y * v._y;
    }

    //! Modulus of vectorial product

    /*! \return \f$ \| v_1 \times v_2 \| = {v_1}_x {v_2}_y - {v_2}_x {v_1}_y \f$ */
    double operator ^ (const Vector2D & v) const
    {
        return _x * v._y - _y * v._x;
    }

    //! Scalar component of \c this in direction of \p u
    double operator/(const Versor2D & u) const;

    //! Vectorial component of \c this in direction of \p u
    const Vector2D operator|(const Versor2D & u) const;

    //! Compound | assigment
    /*! \sa operator |(const Versor2D & u) const */
    Vector2D & operator|=(const Versor2D & u);

    //! Vectorial component of \c this in direction orthogonal of \p u

    /*! \note Operators '*'(field), '/' and '%' are defined so that:
     *  \n <tt> v == u*(v/u) + (v%u) </tt> */
    const Vector2D operator%(const Versor2D & u) const
    {
        return *this -(*this | u);
    }

    //! Compound assignment
    /*! \sa operator %(const Versor2D & u) const */
    Vector2D & operator%=(const Versor2D & u);

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};




//! Point in \f$ \Re^2 \f$

/*! Point is stored using its cartesian components namely \a x and \a y */
class Point2D : public cxu::Serialization<Point2D>
{
    CXU_DECLARE_SERIALIZABLE(Point2D);

private:
    double _x, _y;


public:

    //! Default constructor, sets \a x and \a y components to NaN

    Point2D() : _x(double()), _y(double())
    {
    }

    //! User constructor, sets \a x and \a y components to \p px and \p py

    Point2D(double px, double py) : _x(px), _y(py)
    {
    }

    //! Construct point from vector

    /*! \note Constructed point equals to <tt> Point2D::O + v </tt> */
    explicit Point2D(const Vector2D & v) : _x(v.x()), _y(v.y())
    {
    }

    static const Point2D NaN;

    //! \a x component

    double x() const
    {
        return _x;
    }

    //! \a y component

    double y() const
    {
        return _y;
    }

    //! Delta between \p this and \p p
    /*! \return \f$ v_r \models p_1 - p_2 \f$
     *  \n \f$ {v_r}_x = {p_1}_x - {p_2}_x \f$
     *  \n \f$ {v_r}_y = {p_1}_y - {p_2}_y \f$
     */
    const Vector2D operator-(const Point2D & p) const;

    //! Compute point \c this translated by \p v

    /*! \return \f$ p_r \models p_0 + v \f$
     *  \n \f$ {p_r}_x = {p_0}_x + v_x \f$
     *  \n \f$ {p_r}_y = {p_0}_y + v_y \f$
     */
    const Point2D operator+(const Vector2D & v) const
    {
        return Point2D(_x + v.x(), _y + v.y());
    }

    //! Compound assignment
    /*! \sa operator + (const Vector2D & v) const */
    Point2D & operator+=(const Vector2D & v);

    //! Compute point \c this translated by \p v

    /*! \return \f$ p_0 + (-v) \f$ */
    const Point2D operator-(const Vector2D & v) const
    {
        return Point2D(_x - v.x(), _y - v.y());
    }

    //! Compound assignment
    /*! \sa operator - (const Vector2D & v) const */
    Point2D & operator-=(const Vector2D & v);

    //!Compute \c this point transformated by \p t
    const Point2D operator*(const Transform2D & t) const;

    //! Compound assignment
    /*! \sa operator * (const Transform2D & t) const */
    Point2D & operator*=(const Transform2D & t);

    //! Compute projection of \c this point to line \p l
    const Point2D operator|(const Line2D &) const;

    //! Compound assignment
    /*! \sa operator | (const Line2D & l) const */
    Point2D & operator|=(const Line2D & l);

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

    bool isFinite() const
    {
        return ::finite(_x) && ::finite(_y);
    }

};

//! Compute squared distance between \p p1 and \p p2

/*! \return \f$ |{p_2}_x - {p_1}_x|^2 + |{p_2}_y - {p_1}_y|^2 \f$
 *  \note This function is faster than \c dist because it does not reuqire \c sqrt */
inline double dist2(const Point2D & p1, const Point2D & p2)
{
    return (p1 - p2).qdr();
}

//! Compute distance between \p p1 and \p p2

/*! \return \f$ \sqrt{|{p_2}_x - {p_1}_x|^2 + |{p_2}_y - {p_1}_y|^2} \f$ */
inline double dist(const Point2D & p1, const Point2D & p2)
{
    return ::sqrt(dist2(p1, p2));
}


//! Versor in \f$ \Re^2 \f$

/*! Versor is stored using its cartesian components namely \a x and \a y
 *  always true : \f$ x^2+y^2=1 \f$
 */
class Versor2D : public cxu::Serialization<Versor2D>
{
    CXU_DECLARE_SERIALIZABLE(Versor2D)

private:
    double _x, _y;

    Versor2D(double px, double py, void*) : _x(px), _y(py)
    {
    }

public:

    //! Default constructor

    /*! Set both components to NaN */
    Versor2D() : _x(double()), _y(double())
    {
    }

    //! User constructor

    /*! Constructs versor in the same direction of \f$ \langle px,py \rangle \f$
     *  \pre \f$ px \ne 0 \lor py \ne 0 \f$
     *  \post \f$ {{u_0}_x}^2+{{u_0}_y}^2 = 1, \langle {u_0}_x,{u_0}_y \rangle \sim \langle px,py \rangle \f$
     */
    Versor2D(double px, double py) : _x(px), _y(py)
    {
        normalize();
    }

    //! Construct from vector

    /*! Construct versor in the same direction of \p v
     *  \pre \f$ \|v\| \ne 0 \f$
     *  \post \f$ {{u_0}_x}^2+{{u_0}_y}^2 = 1, \langle {u_0}_x,{u_0}_y \rangle \sim \langle v_x,v_y \rangle \f$
     */
    explicit Versor2D(const Vector2D & v) : _x(v.x()), _y(v.y())
    {
        normalize();
    }

    //! Construct versor given angle frm x positive axis.

    explicit Versor2D(double arg) : _x(cos(arg)), _y(sin(arg))
    {
    }

    //! \a x component

    double x() const
    {
        return _x;
    }

    //! \a y component

    double y() const
    {
        return _y;
    }

    //void set(double px, double py) {_x=px,_y=py;normalize();}

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

    //! x positive semiaxis versor
    static const Versor2D X;

    //! y positive semiaxis versor
    static const Versor2D Y;

    //! Vector in same direction of \c this versor and module \p k

    const Vector2D operator*(double k) const
    {
        return Vector2D(_x*k, _y * k);
    }
    //! Vector in same direction of \c this versor and module <tt> 1/k </tt>

    const Vector2D operator/(double k) const
    {
        return Vector2D(_x / k, _y / k);
    }

    //! Opposite

    const Versor2D operator-() const
    {
        return Versor2D(-_x, -_y, NULL);
    }

    //! Orthogonal

    const Versor2D ort() const
    {
        return Versor2D(-_y, _x, NULL);
    }

    //! Bisector

    static const Versor2D bis(const Versor2D & v1, const Versor2D & v2)
    {
        return Versor2D(v1._x + v2._x, v1._y + v2._y);
    }


    //! Argument

    double operator~() const
    {
        return atan2(_y, _x);
    }

    //! Check if this Versor is finite (not NaN or inf)

    bool isFinite() const
    {
        return ::finite(_x) && ::finite(_y);
    }

private:

    void normalize()
    {
        double k = 1.0 / ::hypot(_x, _y);
        _x *= k, _y *= k;
    }

};


//! Size in \f$ \Re^2 \f$

/*! Size is stored using its cartesian components namely \a x and \a y
 *  always true : \f$ x \ge 0 \land y \ge 0 \f$
 */
class Size2D : public cxu::Serialization<Size2D>
{
    CXU_DECLARE_SERIALIZABLE(Size2D)

private:
    double _x, _y;

public:

    //! Deafault constructor

    /*! Set both components to 0 */
    Size2D() : _x(0), _y(0)
    {
    }

    //! User constrcutor

    Size2D(double px, double py) : _x(fabs(px)), _y(fabs(py))
    {
    }

    //! Constructor from Vector

    explicit Size2D(const Vector2D & v) : _x(fabs(v.x())), _y(fabs(v.y()))
    {
    }

    //! \a x component

    double x() const
    {
        return _x;
    }

    //! \a y component

    double y() const
    {
        return _y;
    }

    //void set(double px, double py) {_x=fabs(px),_y=fabs(py);}
    //void setX(double x) {_x=fabs(x);}
    //void setY(double y) {_y=fabs(y);}

    //! Area

    double area() const
    {
        return _x*_y;
    }

    //! Scalar

    const Size2D operator*(double k) const
    {
        return Size2D(_x*k, _y * k);
    }

    //! Scalar

    const Size2D operator/(double k) const
    {
        return Size2D(_x / k, _y / k);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);


protected:

    void absolutize()
    {
        _x = fabs(_x), _y = fabs(_y);
    }

};


//! Matrix \f$ 2 \times 2 in \Re \f$

/*! Matrix is stored as \f$ \matrix m_11 m_12 m_21 m_22 \f$ */
class Matrix22 : public cxu::Serialization<Matrix22>
{
    CXU_DECLARE_SERIALIZABLE(Matrix22);

private:
    double _m11, _m12, _m21, _m22;

public:

    Matrix22()
    {
        _m11 = 0, _m12 = 0, _m21 = 0, _m22 = 0;
    }

    Matrix22(double m11, double m12, double m21, double m22) : _m11(m11), _m12(m12), _m21(m21), _m22(m22)
    {
    }

    Matrix22(const Vector2D & vc1, const Vector2D & vc2) : _m11(vc1.x()), _m12(vc2.x()), _m21(vc1.y()), _m22(vc2.y())
    {
    }

    //!	Determinant

    double operator!() const
    {
        return _m11 * _m22 - _m12*_m21;
    }

    //! Transposed matrix

    const Matrix22 operator*() const
    {
        return Matrix22(_m11, _m21, _m12, _m22);
    }

    //! Inverse matrix

    const Matrix22 operator~() const
    {
        return Matrix22(_m22, -_m12, -_m21, _m11) / (this->operator!());
    }

    //! Unary plus (NOP) since <tt> m == +m </tt>

    const Matrix22 & operator+() const
    {
        return *this;
    }

    //! Opposite matrix

    const Matrix22 operator-() const
    {
        return Matrix22(-_m11, -_m12, -_m21, -_m22);
    }

    //! Identity
    static const Matrix22 I;

    //! Null
    static const Matrix22 O;

    //! Scalar

    const Matrix22 operator*(double k) const
    {
        return Matrix22(_m11*k, _m12*k, _m21*k, _m22 * k);
    }

    //! Scalar

    const Matrix22 operator/(double k) const
    {
        return Matrix22(_m11 / k, _m12 / k, _m21 / k, _m22 / k);
    }

    //! RowColumn matrix matrix product

    const Matrix22 operator*(const Matrix22 & a) const
    {
        return Matrix22(_m11 * a._m11 + _m12 * a._m21, _m11 * a._m12 + _m12 * a._m22, _m21 * a._m11 + _m22 * a._m21, _m21 * a._m12 + _m22 * a._m22);
    }

    //! Sum

    const Matrix22 operator+(const Matrix22 & a) const
    {
        return Matrix22(_m11 + a._m11, _m12 + a._m12, _m21 + a._m21, _m22 + a._m22);
    }

    //! Difference

    /*! /f$ A - B = A + (-B) /f$ */
    const Matrix22 operator-(const Matrix22 & a) const
    {
        return Matrix22(_m11 - a._m11, _m12 - a._m12, _m21 - a._m21, _m22 - a._m22);
    }

    //! RowColumn matrix vector product

    const Vector2D operator*(const Vector2D & v) const
    {
        return Vector2D(_m11 * v.x() + _m12 * v.y(), _m21 * v.x() + _m22 * v.y());
    }

    //! Rotation matrix given angle
    static const Matrix22 rotation(double theta);

    //! Rotation matrix given new x axis versor
    static const Matrix22 rotation(const Versor2D & vers);

    //! First column vector

    const Vector2D c1() const
    {
        return Vector2D(_m11, _m21);
    }

    //! Second column vector

    const Vector2D c2() const
    {
        return Vector2D(_m12, _m22);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};


//! Segment in \f$ \Re^2 \f$

/*! Segment is stored by its points namely \a x and \a y */
class Segment2D : public cxu::Serialization<Segment2D>
{
    CXU_DECLARE_SERIALIZABLE(Segment2D);

private:
    Point2D _a, _b;

public:

    //! Default constructor
    Segment2D() = default;

    //! Defined constructor

    Segment2D(const Point2D & pa, const Point2D & pb) : _a(pa), _b(pb)
    {
    }

    //! \a a point

    const Point2D & a() const
    {
        return _a;
    }

    //! \a b point

    const Point2D & b() const
    {
        return _b;
    }

    //! Axis
    /*! Line orthogonally crossing this segment in the middle */
    const Line2D axis() const;

    //! Compound projection
    Segment2D & operator|=(const Line2D &);

    //! Projection
    const Segment2D operator|(const Line2D &) const;

    //! Compound translation
    Segment2D & operator+=(const Vector2D & v);

    //! Translation

    const Segment2D operator+(const Vector2D & v) const
    {
        return Segment2D(_a + v, _b + v);
    }

    //! Compound opposite translation
    Segment2D & operator-=(const Vector2D &);

    //! Opposite translation
    const Segment2D operator-(const Vector2D &) const;

    //! Compound linear transform
    Segment2D & operator*=(const Transform2D &);

    //! Linear transform
    const Segment2D operator*(const Transform2D &) const;

    //! Projected containment

    /*! Computes if \c p projected on this segment line is contained in this segment*/
    bool coproj(const Point2D & p) const
    {
        return cxu::inco((p - _a)*(_b - _a), 0.0, length2());
    }

    //! Middle point

    const Point2D middle() const
    {
        return Point2D((_a.x() + _b.x()) / 2.0, (_a.y() + _b.y()) / 2.0);
    }

    //! Length

    double length() const
    {
        return ::sqrt(length2());
    }

    //! Squared length

    double length2() const
    {
        return dist2(_a, _b);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};


//! Line in \f$ \Re^2 \f$

/*! Line is stored by its origin Point and direction Versor */
class Line2D : public cxu::Serialization<Line2D>
{
    CXU_DECLARE_SERIALIZABLE(Line2D);

    friend Point2D inters(const Line2D &, const Line2D &);

private:
    Point2D _p;
    Versor2D _u;

public:

    //! Default constructor
    Line2D() = default;

    //! Defined constructor

    Line2D(const Point2D & p, const Versor2D & u) : _p(p), _u(u)
    {
    }

    //! Line by 2 points

    /*! Origin is set to \c a */
    Line2D(const Point2D & a, const Point2D & b) : _p(a), _u(b - a)
    {
    }

    //! Line of a segment

    /*! Origin is set to \c s.a() */
    explicit Line2D(const Segment2D & s) : _p(s.a()), _u(s.b() - s.a())
    {
    }

    //! Line of a semiline
    explicit Line2D(const SemiLine2D &);

    //! Origin

    const Point2D & org() const
    {
        return _p;
    }

    //! Direction

    const Versor2D & dir() const
    {
        return _u;
    }

    //! Compound translation
    Line2D & operator+=(const Vector2D &);

    //! Translation
    const Line2D operator+(const Vector2D &) const;

    //! Compound opposite translation
    Line2D & operator-=(const Vector2D &);

    //! Opposite translation
    const Line2D operator-(const Vector2D &) const;

    //! Compound transform
    Line2D & operator*=(const Transform2D &);

    //! Transform
    const Line2D operator*(const Transform2D & t) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};


//! SemiLine in \f$ \Re^2 \f$

/*! SemiLine is stored by its origin Point and direction Versor */
class SemiLine2D : public cxu::Serialization<SemiLine2D>
{
    CXU_DECLARE_SERIALIZABLE(SemiLine2D);

private:
    Point2D _p;
    Versor2D _u;

public:

    //! Default constructor
    SemiLine2D() = default;

    //! Defined constructor

    SemiLine2D(const Point2D & p, const Versor2D & u) : _p(p), _u(u)
    {
    }

    //! SemiLine by 2 points

    /*! Origin is set to \c a */
    SemiLine2D(const Point2D& a, const Point2D& b) : _p(a), _u(b - a)
    {
    }

    //! Semiline of a Segment

    /*! Origin is set to \c s.a() */
    explicit SemiLine2D(const Segment2D & s) : _p(s.a()), _u(s.b() - s.a())
    {
    }

    //! Origin

    const Point2D & org() const
    {
        return _p;
    }

    //! Direction

    const Versor2D & dir() const
    {
        return _u;
    }

    //! Projected containment

    /*! Computes if \c p projected on this semiline line is contained in this semiline */
    bool coproj(const Point2D & p) const
    {
        return (p - _p) / _u >= 0;
    }

    //! Compound translation
    SemiLine2D & operator+=(const Vector2D &);

    //! Translation
    const SemiLine2D operator+(const Vector2D &) const;

    //! Compound opposite translation
    SemiLine2D & operator-=(const Vector2D &);

    //! Opposite translation
    const SemiLine2D operator-(const Vector2D &) const;

    //! Compound transform
    SemiLine2D & operator*=(const Transform2D &);

    //! Transform
    const SemiLine2D operator*(const Transform2D &) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};


//! Polygon in \f$ \Re^2 \f$

/*! Polygon is stored as an array of vertices*/
class Polygon2D : public cxu::Serialization<Polygon2D>
{
    CXU_DECLARE_SERIALIZABLE(Polygon2D);

public:

    explicit Polygon2D(size_t n) : _p(n)
    {
    }
    Polygon2D() = default;

    static Polygon2D regular(size_t);


    //! Perimeter
    double length(void) const;

    //! How many times this polygon wraps around p counterclockwise
    int index(const Point2D &) const;

    //! Compute if point \c p is contained in this polygon

    bool co(const Point2D & p) const
    {
        return (index(p) % 2 != 0);
    }

    const Point2D at(double t) const;

    //! Compound translation
    Polygon2D & operator+=(const Vector2D &);

    //! Translation
    Polygon2D operator+(const Vector2D &) const;

    //! Compound opposite translation
    Polygon2D & operator-=(const Vector2D &);

    //! Opposite translation
    Polygon2D operator-(const Vector2D &) const;

    //! Compound transform
    Polygon2D & operator*=(const Transform2D &);

    //! Transform
    Polygon2D operator*(const Transform2D &) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

    std::vector<Point2D> & points()
    {
        return _p;
    }

    const std::vector<Point2D> & points() const
    {
        return _p;
    }


    //! Area
    double area() const;

    //! Baricenter
    const Point2D baricenter() const;


private:

    std::vector<Point2D> _p;
    double length(size_t) const;
    const Point2D vcenter() const;

};


//! Linear affine transform in \f$ \Re^2 \f$

/*! Transform is stored as a linear transform matrix and translation vector */
class Transform2D : public cxu::Serialization<Transform2D>
{
    CXU_DECLARE_SERIALIZABLE(Transform2D);

private:

    Matrix22 _matrix;
    Vector2D _vector;


public:

    //! Deafaul constructor

    /*! Identity transform */
    Transform2D() : _matrix(Matrix22::I)
    {
    }

    //! Definite constrcutor

    Transform2D(const Matrix22 & m, const Vector2D & v) : _matrix(m), _vector(v)
    {
    }

    //! Inverse transforamtion
    const Transform2D operator~() const;

    //! Scale double factor

    double operator!() const
    {
        return !_matrix;
    }

    //! Composition
    const Transform2D operator*(const Transform2D &) const;

    //! Compound composition
    Transform2D & operator*=(const Transform2D &);

    const Point2D apply(const Point2D &) const;

    //! Translation by vector
    static const Transform2D translation(const Vector2D &);

    //! Rotation by angle centered in origin
    static const Transform2D rotation(double alfa);

    //! Rotation by new x axis versor centered in origin
    static const Transform2D rotation(const Versor2D & n);

    //! Rotation by angle centered in point
    static const Transform2D rotation(double alfa, const Point2D & center);

    //! Rotation by new x axis versor centered in point
    static const Transform2D rotation(const Versor2D & dir, const Point2D & center);

    //! Scale
    static const Transform2D scale(const Vector2D & k);

    const Matrix22 & matrix() const
    {
        return _matrix;
    }

    const Vector2D & vector() const
    {
        return _vector;
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};




//------------------------------------------------


//! sinus of the angle between \c u1 and \c u2

inline double sin(const Versor2D & u1, const Versor2D & u2)
{
    return (u1.x() * u2.y() - u1.y() * u2.x());
}
//! cosinus of the angle between \c u1 and \c u2

inline double cos(const Versor2D & u1, const Versor2D & u2)
{
    return (u1.x() * u2.x() + u1.y() * u2.y());
}

//! sinus of the angle between \c v1 and \c v2

inline double sin(const Vector2D & v1, const Vector2D & v2)
{
    return (v1^v2) / ::sqrt(v1.qdr() * v2.qdr());
}
//! cosinus of the angle between \c v1 and \c v2

inline double cos(const Vector2D & v1, const Vector2D & v2)
{
    return (v1 * v2) / ::sqrt(v1.qdr() * v2.qdr());
}

//! angle between \c u1 and \c u2
double angle(const Versor2D & u1, const Versor2D & u2);

//! Intersection between two lines
Point2D inters(const Line2D & r1, const Line2D & r2);

//! Intersection between two segments
/*! NaN if no intersection */
Point2D inters(const Segment2D & s1, const Segment2D & s2);

//! Collision test between two segments
bool collide(const Segment2D & s1, const Segment2D & s2);

//! Intersection between a segment and a line
/*! NaN if no intersection */
Point2D inters(const Segment2D & s, const Line2D & r);

//! Intersection between a segment and a semiline
/*! NaN if no intersection */
Point2D inters(const Segment2D & s, const SemiLine2D & r);

//! Collision between a segment and a semiline
bool collide(const Segment2D & s, const SemiLine2D & r);

//! Intersections between polygon and segment
std::vector<Point2D> inters(const Polygon2D &, const Segment2D &);

//! Intersections between polygon and semiline
std::vector<Point2D> inters(const Polygon2D &, const SemiLine2D &);

//! Squared distance between point and line

inline double dist2(const Point2D & p, const Line2D & r)
{
    return dist2(p, p | r);
}

//! Distance between point and line

inline double dist(const Point2D & p, const Line2D & r)
{
    return ::sqrt(dist2(p, r));
}

//! Squared distance between point and segment
double dist2(const Point2D & p, const Segment2D & s);

//! Distance between point and segment

inline double dist(const Point2D & p, const Segment2D & s)
{
    return ::sqrt(dist2(p, s));
}

//! Squared distance between two segments
double dist2(const Segment2D & s1, const Segment2D & s2);
//! Distance between two segments

inline double dist(const Segment2D & s1, const Segment2D & s2)
{
    return ::sqrt(dist2(s1, s2));
}

//! Squared distance between two polygons
/*! \note If a polygon is fully contained in the other, distance is 0 */
double dist2(const Polygon2D &, const Polygon2D &);

//! Distance between two polygons

inline double dist(const Polygon2D & p1, const Polygon2D & p2)
{
    return ::sqrt(dist2(p1, p2));
}

//! Collision between two polygons
bool collide(const Polygon2D &, const Polygon2D &);

//! Intersections between two polygons
std::vector<Point2D> inters(const Polygon2D &, const Polygon2D &);

//! ConvexHull algorithm
std::vector<Point2D> convexHull(const std::vector<Point2D> & p, std::set<size_t> * used = 0);




CXM_NS_END

#endif
