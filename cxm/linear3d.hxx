// YAL zeldan

#ifndef CXM_LINEAR3D_HXX
#define CXM_LINEAR3D_HXX


#include "linear2d.hxx"
#include "defs.hxx"

CXM_NS_BEGIN

//! Vector in \f$\Re^3\f$
/*! Vector is stored using its cartesian components namely \a x, \a y and \a z.
 *  \n This class in nearly-immutable: only serialize, assignment and compound assignment can change its state.
 *  \n Hereafter in this page:
 *  - v is used for vector
 *  - u is used for versor
 *  - k is used for scalar
 *  .
 *  - subsctipt 0 is used for \c this vector
 *  - subscript 1,2..n are used for parameters and for binary operators
 *  - subscript r is used for return value
 *  .
 */

class Vector3D : public cxu::Serialization<Vector3D> /// Vettore in 3D
{
    CXU_DECLARE_SERIALIZABLE(Vector3D)

private:
    double _x, _y, _z;

public:

    Vector3D() : _x(0), _y(0), _z(0)
    {
    }

    Vector3D(double px, double py, double pz) : _x(px), _y(py), _z(pz)
    {
    }
    explicit Vector3D(const Point3D & p);
    explicit Vector3D(const Versor3D & v);

    double x() const
    {
        return _x;
    }

    double y() const
    {
        return _y;
    }

    double z() const
    {
        return _z;
    }

    double operator!() const
    {
        return ::sqrt(qdr());
    }

    double qdr() const
    {
        return (cxu::sqr(_x) + cxu::sqr(_y) + cxu::sqr(_z));
    }

    const Vector3D & operator+() const
    {
        return *this;
    }

    const Vector3D operator-() const
    {
        return Vector3D(-_x, -_y, -_z);
    }
    static const Vector3D O;

    const Vector3D operator*(double k) const
    {
        return Vector3D(_x*k, _y*k, _z * k);
    }

    const Vector3D operator/(double k) const
    {
        return Vector3D(_x / k, _y / k, _z / k);
    }

    Vector3D & operator+=(const Vector3D & v);
    Vector3D & operator-=(const Vector3D & v);
    Vector3D & operator*=(const Matrix33 & m);

    double operator*(const Vector3D & v) const
    {
        return _x * v._x + _y * v._y + _z * v._z;
    }
    const Vector3D operator ^ (const Vector3D & v) const;

    const Vector3D operator+(const Vector3D & v) const
    {
        return Vector3D(_x + v._x, _y + v._y, _z + v._z);
    }

    const Vector3D operator-(const Vector3D & v) const
    {
        return Vector3D(_x - v._x, _y - v._y, _z - v._z);
    }

    const Vector3D operator|(const Versor3D & u) const;
    Vector3D & operator|=(const Versor3D & u);

    double operator/(const Versor3D &) const;
    const Vector3D operator%(const Versor3D &) const;
    Vector3D & operator%=(const Versor3D &);

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Point3D : public cxu::Serialization<Point3D>
{
    CXU_DECLARE_SERIALIZABLE(Point3D)

private:
    double _x, _y, _z;

public:

    Point3D() : _x(double()), _y(double()), _z(double())
    {
    }

    Point3D(double px, double py, double pz) : _x(px), _y(py), _z(pz)
    {
    }

    explicit Point3D(const Vector3D & v) : _x(v.x()), _y(v.y()), _z(v.z())
    {
    }

    static const Point3D NaN;

    double x() const
    {
        return _x;
    }

    double y() const
    {
        return _y;
    }

    double z() const
    {
        return _z;
    }

    const Point3D operator|(const Line3D &) const;
    const Point3D operator|(const Plane3D &) const;
    Point3D & operator|=(const Line3D &);
    Point3D & operator|=(const Plane3D &);

    Point3D & operator+=(const Vector3D & v);
    const Point3D operator+(const Vector3D & v) const;

    Point3D & operator-=(const Vector3D & v);
    const Point3D operator-(const Vector3D & v) const;

    Point3D & operator*=(const Transform3D & v);
    const Point3D operator*(const Transform3D & v) const;

    const Vector3D operator-(const Point3D & p) const;

    bool isFinite() const
    {
        return ::finite(_x) && ::finite(_y) && ::finite(_z);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

inline double dist2(const Point3D & p1, const Point3D & p2)
{
    return (p2 - p1).qdr();
}

inline double dist(const Point3D & p1, const Point3D & p2)
{
    return ::sqrt(dist2(p1, p2));
}

class Matrix33 : public cxu::Serialization<Matrix33>
{
    CXU_DECLARE_SERIALIZABLE(Matrix33)

public:

    double _m11, _m12, _m13, _m21, _m22, _m23, _m31, _m32, _m33;

    Matrix33() :
    _m11(0), _m12(0), _m13(0),
    _m21(0), _m22(0), _m23(0),
    _m31(0), _m32(0), _m33(0)
    {
    }

    Matrix33(double m11, double m12, double m13,
             double m21, double m22, double m23,
             double m31, double m32, double m33) :
    _m11(m11), _m12(m12), _m13(m13),
    _m21(m21), _m22(m22), _m23(m23),
    _m31(m31), _m32(m32), _m33(m33)
    {
    }

    Matrix33(const Vector3D & pc1, const Vector3D & pc2, const Vector3D & pc3) :

    _m11(pc1.x()), _m12(pc2.x()), _m13(pc3.x()),
    _m21(pc1.y()), _m22(pc2.y()), _m23(pc3.y()),
    _m31(pc1.z()), _m32(pc2.z()), _m33(pc3.z())
    {
    }

    double operator!() const
    {
        return
        +(_m11 * _m22 * _m33)
                - (_m11 * _m23 * _m32)
                - (_m12 * _m21 * _m33)
                + (_m12 * _m23 * _m31)
                + (_m13 * _m21 * _m32)
                - (_m13 * _m22 * _m31);
    }

    const Matrix33 operator*() const
    {
        return Matrix33(_m11, _m21, _m31, _m12, _m22, _m32, _m13, _m23, _m33);
    }
    const Matrix33 operator~() const;

    const Matrix33 & operator+() const
    {
        return *this;
    }

    const Matrix33 operator-() const
    {
        return Matrix33(-_m11, -_m12, -_m13, -_m21, -_m22, -_m23, -_m31, -_m32, -_m33);
    }

    const Matrix22 minor11() const
    {
        return Matrix22(_m22, _m23, _m32, _m33);
    }

    const Matrix22 minor12() const
    {
        return Matrix22(_m21, _m23, _m31, _m33);
    }

    const Matrix22 minor13() const
    {
        return Matrix22(_m21, _m22, _m31, _m32);
    }

    const Matrix22 minor21() const
    {
        return Matrix22(_m12, _m13, _m32, _m33);
    }

    const Matrix22 minor22() const
    {
        return Matrix22(_m11, _m13, _m31, _m33);
    }

    const Matrix22 minor23() const
    {
        return Matrix22(_m11, _m12, _m31, _m32);
    }

    const Matrix22 minor31() const
    {
        return Matrix22(_m12, _m13, _m22, _m23);
    }

    const Matrix22 minor32() const
    {
        return Matrix22(_m11, _m13, _m21, _m23);
    }

    const Matrix22 minor33() const
    {
        return Matrix22(_m11, _m12, _m21, _m22);
    }

    static const Matrix33 I;
    static const Matrix33 O;

    const Vector3D operator*(const Vector3D & v) const;
    //const Versor3D operator * (const Versor3D &) const;

    const Matrix33 operator*(double k) const
    {
        return Matrix33(_m11*k, _m12*k, _m13*k, _m21*k, _m22*k, _m23*k, _m31*k, _m32*k, _m33 * k);
    }

    const Matrix33 operator/(double k) const
    {
        return Matrix33(_m11 / k, _m12 / k, _m13 / k, _m21 / k, _m22 / k, _m23 / k, _m31 / k, _m32 / k, _m33 / k);
    }

    const Matrix33 operator*(const Matrix33 & m) const;
    const Matrix33 operator+(const Matrix33 & m) const;
    const Matrix33 operator-(const Matrix33 & m) const;

    Matrix33 & operator+=(const Matrix33 & m);
    Matrix33 & operator-=(const Matrix33 & m);

    static const Matrix33 rotation(double theta, const Versor3D & n)
    {
        return rotation(::cos(theta), ::sin(theta), n);
    }
    static const Matrix33 rotation(double costheta, double sintheta, const Versor3D & n);

    const Vector3D c1() const
    {
        return Vector3D(_m11, _m21, _m31);
    }

    const Vector3D c2() const
    {
        return Vector3D(_m12, _m22, _m32);
    }

    const Vector3D c3() const
    {
        return Vector3D(_m13, _m23, _m33);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Versor3D : public cxu::Serialization<Versor3D>
{
    CXU_DECLARE_SERIALIZABLE(Versor3D)

private:
    double _x, _y, _z;

    void normalize()
    {
        double k = 1.0 / ::sqrt(cxu::sqr(_x) + cxu::sqr(_y) + cxu::sqr(_z));
        _x *= k, _y *= k, _z *= k;
    }

    Versor3D(double px, double py, double pz, void*) : _x(px), _y(py), _z(pz)
    {
    }

public:

    Versor3D() : _x(double()), _y(double()), _z(double())
    {
    }

    Versor3D(double px, double py, double pz) : _x(px), _y(py), _z(pz)
    {
        normalize();
    }

    explicit Versor3D(const Vector3D & v) : _x(v.x()), _y(v.y()), _z(v.z())
    {
        normalize();
    }

    //void set(double x, double y, double z) {_x=x,_y=y,_z=z;normalize();}
    //void setX(double x) {_x=x;normalize();}
    //void setY(double y) {_y=y;normalize();}
    //void setZ(double z) {_z=z;normalize();}

    double x() const
    {
        return _x;
    }

    double y() const
    {
        return _y;
    }

    double z() const
    {
        return _z;
    }

    static const Versor3D X;
    static const Versor3D Y;
    static const Versor3D Z;

    static const Versor3D ort(const Versor3D & v1, const Versor3D & v2)
    {
        return Versor3D(Vector3D(v1)^Vector3D(v2));
    }

    static const Versor3D bisect(const Versor3D & v1, const Versor3D & v2)
    {
        return Versor3D(Vector3D(v1) + Vector3D(v2));
    }

    //! Opposite

    const Versor3D operator-() const
    {
        return Versor3D(-_x, -_y, -_z, NULL);
    }

    const Vector3D operator*(double k) const
    {
        return Vector3D(_x*k, _y*k, _z * k);
    }

    const Vector3D operator/(double k) const
    {
        return Vector3D(_x / k, _y / k, _z / k);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Line3D : public cxu::Serialization<Line3D>
{
    CXU_DECLARE_SERIALIZABLE(Line3D)

private:
    Point3D _p;
    Versor3D _u;

public:
    Line3D() = default;

    Line3D(const Point3D & p, const Versor3D & u) : _p(p), _u(u)
    {
    }
    Line3D(const Point3D & p1, const Point3D & p2);
    explicit Line3D(const Segment3D &);
    explicit Line3D(const SemiLine3D &);

    const Point3D & org() const
    {
        return _p;
    }

    const Versor3D & dir() const
    {
        return _u;
    }

    const Line3D operator|(const Plane3D &) const;
    Line3D & operator|=(const Plane3D &);

    Line3D & operator+=(const Vector3D & v);
    const Line3D operator+(const Vector3D & v) const;

    Line3D & operator-=(const Vector3D & v);
    const Line3D operator-(const Vector3D & v) const;

    Line3D & operator*=(const Transform3D & v);
    const Line3D operator*(const Transform3D & v) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class SemiLine3D : public cxu::Serialization<SemiLine3D>
{
    CXU_DECLARE_SERIALIZABLE(SemiLine3D)

private:
    Point3D _p;
    Versor3D _u;

public:
    SemiLine3D() = default;

    SemiLine3D(const Point3D & p, const Versor3D & u) : _p(p), _u(u)
    {
    }
    SemiLine3D(const Point3D & p1, const Point3D & p2);
    explicit SemiLine3D(const Segment3D &);

    const Point3D & org() const
    {
        return _p;
    }

    const Versor3D & dir() const
    {
        return _u;
    }

    const SemiLine3D operator|(const Plane3D &) const;
    SemiLine3D & operator|=(const Plane3D &);

    SemiLine3D & operator+=(const Vector3D & v);
    const SemiLine3D operator+(const Vector3D & v) const;

    SemiLine3D & operator-=(const Vector3D & v);
    const SemiLine3D operator-(const Vector3D & v) const;

    SemiLine3D & operator*=(const Transform3D & v);
    const SemiLine3D operator*(const Transform3D & v) const;

    bool coproj(const Point3D & q) const
    {
        return ((q - _p) / _u >= 0);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Plane3D : public cxu::Serialization<Plane3D>
{
    CXU_DECLARE_SERIALIZABLE(Plane3D)

private:
    Point3D _p;
    Versor3D _n;

public:
    Plane3D() = default;

    Plane3D(const Point3D & p, const Versor3D & n) : _p(p), _n(n)
    {
    }
    explicit Plane3D(const Triangle3D & t);

    const Point3D & org() const
    {
        return _p;
    }

    const Versor3D & nrm() const
    {
        return _n;
    }

    Plane3D & operator+=(const Vector3D & v);
    const Plane3D operator+(const Vector3D & v) const;

    Plane3D & operator-=(const Vector3D & v);
    const Plane3D operator-(const Vector3D & v) const;

    Plane3D & operator*=(const Transform3D & v);
    const Plane3D operator*(const Transform3D & v) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};

class Segment3D : public cxu::Serialization<Segment3D>
{
    CXU_DECLARE_SERIALIZABLE(Segment3D)

private:
    Point3D _a, _b;

public:
    Segment3D() = default;

    Segment3D(const Point3D & pa, const Point3D & pb) : _a(pa), _b(pb)
    {
    }

    const Point3D & a() const
    {
        return _a;
    }

    const Point3D & b() const
    {
        return _b;
    }

    Segment3D & operator+=(const Vector3D & v);
    const Segment3D operator+(const Vector3D & v) const;

    Segment3D & operator-=(const Vector3D & v);
    const Segment3D operator-(const Vector3D & v) const;

    Segment3D & operator*=(const Transform3D & v);
    const Segment3D operator*(const Transform3D & v) const;

    const Segment3D operator|(const Line3D &) const;
    const Segment3D operator|(const Plane3D &) const;
    Segment3D & operator|=(const Line3D &);
    Segment3D & operator|=(const Plane3D &);

    bool coproj(const Point3D & p) const
    {
        return cxu::inco((p - _a)*(_b - _a), 0.0, length2());
    }
    const Point3D middle() const;

    double length() const
    {
        return ::sqrt(length2());
    }

    double length2() const
    {
        return dist2(_a, _b);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Triangle3D : public cxu::Serialization<Triangle3D>
{
    CXU_DECLARE_SERIALIZABLE(Triangle3D)

public:
    Point3D _a, _b, _c;

    Triangle3D() = default;

    Triangle3D(const Point3D & pa, const Point3D & pb, const Point3D & pc) : _a(pa), _b(pb), _c(pc)
    {
    }

    const Point3D & a() const
    {
        return _a;
    }

    const Point3D & b() const
    {
        return _b;
    }

    const Point3D & c() const
    {
        return _c;
    }

    const Point3D baricenter() const;
    const Vector3D operator!() const;

    Triangle3D & operator+=(const Vector3D & v);
    const Triangle3D operator+(const Vector3D & v) const;

    Triangle3D & operator-=(const Vector3D & v);
    const Triangle3D operator-(const Vector3D & v) const;

    Triangle3D & operator*=(const Transform3D & v);
    const Triangle3D operator*(const Transform3D & v) const;

    const Triangle3D operator|(const Plane3D &) const;
    Triangle3D & operator|=(const Plane3D &);

    bool coproj(const Point3D & p) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Size3D : public cxu::Serialization<Size3D>
{
    CXU_DECLARE_SERIALIZABLE(Size3D)

private:
    double _x, _y, _z;

    void absolutize()
    {
        _x = fabs(_x), _y = fabs(_y), _z = fabs(_z);
    }

public:

    Size3D() : _x(0), _y(0), _z(0)
    {
    }

    Size3D(double x, double y, double z) : _x(fabs(x)), _y(fabs(y)), _z(fabs(z))
    {
    }

    //void set(double x, double y, double z) {_x=fabs(x),_y=fabs(y),_z=fabs(z);}
    //void setX(double x) {_x=fabs(x);}
    //void setY(double y) {_y=fabs(y);}
    //void setZ(double z) {_z=fabs(z);}

    //double getX() const {return _x;}
    //double getY() const {return _y;}
    //double getZ() const {return _z;}

    double volume() const
    {
        return _x * _y*_z;
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Tetrahedron : public cxu::Serialization<Tetrahedron>
{
    CXU_DECLARE_SERIALIZABLE(Tetrahedron)

private:
    Point3D _a, _b, _c, _d;

public:

    Tetrahedron(const Point3D & a, const Point3D & b, const Point3D & c, const Point3D & d) :
    _a(a), _b(b), _c(c), _d(d)
    {
    }

    const Point3D baricenter() const
    {
        return Point3D((_a.x() + _b.x() + _c.x() + _d.x()) / 4, (_a.y() + _b.y() + _c.y() + _d.y()) / 4, (_a.z() + _b.z() + _c.z() + _d.z()) / 4);
    }

    double volume() const
    {
        return !Matrix33(_b - _a, _c - _a, _d - _a) / 3;
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Solid3D : public cxu::Serialization<Solid3D>
{ /// solido memorizzato tramite le facce triangolari

    CXU_DECLARE_SERIALIZABLE(Solid3D)

public:
    typedef std::vector<Point3D>::size_type t_ndx;

private:

    class Face : public cxu::Serialization<Face>
    {
        CXU_DECLARE_SERIALIZABLE(Face)
    public:
        t_ndx _n1, _n2, _n3;

        Face() : _n1(0), _n2(0), _n3(0)
        {
        }

        Face(t_ndx n1, t_ndx n2, t_ndx n3) : _n1(n1), _n2(n2), _n3(n3)
        {
        }

        void _Serialization_serialize(cxu::BOStorage &) const;
        void _Serialization_serialize(cxu::BIStorage &);
        void _Serialization_serialize(cxu::TOStorage &) const;
        void _Serialization_serialize(cxu::TIStorage &);
    };

    std::vector<Point3D> _vertices;
    std::vector<Face> _faces;

    mutable std::vector<Triangle3D> _cache_faces;
    mutable std::vector<Segment3D> _cache_edges;
    mutable bool _cache_ok_faces;
    mutable bool _cache_ok_edges;

    const std::set<std::pair<t_ndx, t_ndx> > extractEdges() const;
    void computeFaces() const;
    void computeEdges() const;

    void invalidate()
    {
        _cache_ok_faces = false, _cache_ok_edges = false;
    }
    const Point3D vcenter() const;


public:

    Solid3D() : _cache_ok_faces(false), _cache_ok_edges(false)
    {
    }

    t_ndx addVertex(const Point3D & t);
    void addFace(t_ndx n1, t_ndx n2, t_ndx n3);

    bool verify() const;
    const std::vector<Segment3D> & edges() const;
    const std::vector<Triangle3D> & faces() const;

    Solid3D & operator+=(const Vector3D & v);
    const Solid3D operator+(const Vector3D & v) const;

    Solid3D & operator-=(const Vector3D & v);
    const Solid3D operator-(const Vector3D & v) const;

    Solid3D & operator*=(const Transform3D & v);
    const Solid3D operator*(const Transform3D & v) const;

    const Point3D baricenter() const;
    double volume() const;
    double area() const;

    bool co(const Point3D & p) const;
    void clear();

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};

class Transform3D : public cxu::Serialization<Transform3D>
{

    CXU_DECLARE_SERIALIZABLE(Transform3D)

public:

    Transform3D() : _matrix(Matrix33::I)
    {
    }

    Transform3D(const Matrix33 & m, const Vector3D & v) : _matrix(m), _vector(v)
    {
    }

    const Transform3D operator~() const;
    const Transform3D operator-() const;
    const Transform3D operator+() const;
    double operator!() const;
    const Transform3D operator*(const Transform3D &) const;

    const Point3D apply(const Point3D &) const;

    static const Transform3D translation(const Vector3D &);
    static const Transform3D rotation(double alfa, const Versor3D &, const Point3D & center);
    static const Transform3D rotation(const Versor3D & v1, const Versor3D & v2, const Point3D & center);
    static const Transform3D scale(double kappa, const Point3D & center);

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);


private:
    Matrix33 _matrix;
    Vector3D _vector;
};

class Polygon3D : public cxu::Serialization<Polygon3D> /// poligono memorizzato mediante i vertici
{

    CXU_DECLARE_SERIALIZABLE(Polygon3D)

public:
    explicit Polygon3D(size_t n) : _p(n)
    {
    }
    Polygon3D() = default;

    static Polygon3D regular(size_t);

    void resize(size_t n)
    {
        _p.resize(n);
    }

    size_t size() const
    {
        return _p.size();
    }

    const Point3D & operator[](size_t) const;
    Point3D & operator[](size_t);

    double length() const; // perimetro


    Polygon3D & operator+=(const Vector3D &);
    Polygon3D operator+(const Vector3D &) const;

    Polygon3D & operator-=(const Vector3D &);
    Polygon3D operator-(const Vector2D &) const;

    Polygon3D & operator*=(const Transform3D &);
    Polygon3D operator*(const Transform3D &) const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);


private:
    std::vector<Point3D> _p;
};



//*//////////////////////////////// globals

inline double triproduct(const Vector3D & v1, const Vector3D & v2, const Vector3D & v3)
{
    return !Matrix33(v1, v2, v3);
} // determinante della matrice ottenuta dai tre vettori

inline double cos(const Vector3D & v1, const Vector3D & v2)
{
    return (v1 * v2) / ::sqrt(v1.qdr() * v2.qdr());
} // coseno dell' angolo compreso tra i due vettori (commutativo)

inline double cos(const Versor3D & v1, const Versor3D & v2)
{
    return Vector3D(v1) * Vector3D(v2);
} // coseno dell' angolo compreso tra i due vettori (commutativo)

inline double dist2(const Line3D & a, const Line3D & b)
{
    return ((a.org() - b.org()) | Versor3D::ort(a.dir(), b.dir())).qdr();
}

inline double dist(const Line3D & a, const Line3D & b)
{
    return ::sqrt(dist2(a, b));
}

double dist2(const Segment3D &, const Segment3D &);

inline double dist(const Segment3D & a, const Segment3D & b)
{
    return ::sqrt(dist2(a, b));
}

double dist2(const Triangle3D &, const Triangle3D &);

inline double dist(const Triangle3D & a, const Triangle3D & b)
{
    return ::sqrt(dist2(a, b));
}

//double dist2(const Solid3D &, const Solid3D &);
//inline double dist(const Solid3D & a, const Solid3D & b) {return ::sqrt(dist2(a,b));}

inline double dist2(const Point3D & p, const Line3D & r)
{
    return dist2(p, p | r);
}

inline double dist(const Point3D & p, const Line3D & r)
{
    return ::sqrt(dist2(p, r));
}

inline double dist2(const Point3D & p, const Plane3D & k)
{
    return dist2(p, p | k);
}

inline double dist(const Point3D & p, const Plane3D & k)
{
    return ::sqrt(dist2(p, k));
}

double dist2(const Point3D &, const Segment3D &);

inline double dist(const Point3D & p, const Segment3D & s)
{
    return ::sqrt(dist2(p, s));
}

double dist2(const Point3D &, const Triangle3D &);

inline double dist(const Point3D & p, const Triangle3D & t)
{
    return ::sqrt(dist2(p, t));
}

double dist2(const Segment3D &, const Triangle3D &);

inline double dist(const Segment3D & s, const Triangle3D & t)
{
    return ::sqrt(dist2(s, t));
}

//double dist(const Point3D &, const Box3D &);
//double dist(const Box3D &, const Box3D &);


Point3D inters(const Line3D & r, const Plane3D & alfa);
Point3D inters(const SemiLine3D & r, const Triangle3D & t);

Point3D inters(const Segment3D & s, const Triangle3D & t);
bool collide(const Segment3D & s, const Triangle3D & t);

double solidAngle(const Versor3D &, const Versor3D &, const Versor3D &);


CXM_NS_END

#endif
