// YAL zeldan

#define _USE_MATH_DEFINES
#include <cmath>

#include <cxu/cxu.hxx>

#include "linear3d.hxx"

#include "real.hxx"
#include "stats.hxx"

#include <cxu/mymacros.hxx>
#include "mymacros.hxx"

using namespace std;
using namespace cxu;


CXM_NS_BEGIN


        //---------------------------------------------- Vector3D

        const Vector3D Vector3D::O = Vector3D(0, 0, 0);

Vector3D::Vector3D(const Point3D & p) : _x(p.x()), _y(p.y()), _z(p.z())
{
}

Vector3D::Vector3D(const Versor3D & v) : _x(v.x()), _y(v.y()), _z(v.z())
{
}

const Vector3D Vector3D::operator ^ (const Vector3D & v) const
{
    return Vector3D(
                    + !Matrix22(_y, _z, v._y, v._z),
                    - !Matrix22(_x, _z, v._x, v._z),
                    + !Matrix22(_x, _y, v._x, v._y)
                    );
}

Vector3D & Vector3D::operator+=(const Vector3D & v)
{
    _x += v._x, _y += v._y, _z += v._z;
    return *this;
}

Vector3D & Vector3D::operator-=(const Vector3D & v)
{
    return *this += -v;
}

const Vector3D Vector3D::operator|(const Versor3D & u) const
{
    return u * (*this / u);
}

Vector3D & Vector3D::operator|=(const Versor3D & u)
{
    *this = *this | u;
    return *this;
}

double Vector3D::operator/(const Versor3D & v) const
{
    return _x * v.x() + _y * v.y() + _z * v.z();
}

const Vector3D Vector3D::operator%(const Versor3D & v) const
{
    return *this -(*this | v);
}

Vector3D & Vector3D::operator%=(const Versor3D & v)
{
    *this -= (*this | v);
    return *this;
}

Vector3D & Vector3D::operator*=(const Matrix33 & m)
{
    *this = m * * this;
    return *this;
}

void Vector3D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y << _z;
}

void Vector3D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y >> _z;
}

void Vector3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << "," << _z << ")";
}

void Vector3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> "," >> _z >> ")";
}


//----------------------------- Point3D

const Point3D Point3D::NaN = Point3D(
                                     std::numeric_limits<double>::quiet_NaN(),
                                     std::numeric_limits<double>::quiet_NaN(),
                                     std::numeric_limits<double>::quiet_NaN()
                                     );

const Point3D Point3D::operator|(const Line3D & r) const
{
    return r.org() + ((*this -r.org()) | r.dir());
}

const Point3D Point3D::operator|(const Plane3D & k) const
{
    return *this +((k.org() - * this) | k.nrm());
}

Point3D & Point3D::operator|=(const Line3D & r)
{
    *this = *this | r;
    return *this;
}

Point3D & Point3D::operator|=(const Plane3D & k)
{
    *this += ((k.org() - * this) | k.nrm());
    return *this;
}

const Point3D Point3D::operator+(const Vector3D & v) const
{
    return Point3D(_x + v.x(), _y + v.y(), _z + v.z());
}

Point3D & Point3D::operator+=(const Vector3D & v)
{
    _x += v.x(), _y += v.y(), _z += v.z();
    return *this;
}

const Point3D Point3D::operator-(const Vector3D & v) const
{
    return *this + -v;
}

Point3D & Point3D::operator-=(const Vector3D & v)
{
    return *this += -v;
}

const Point3D Point3D::operator*(const Transform3D & t) const
{
    return t.apply(*this);
}

Point3D & Point3D::operator*=(const Transform3D & t)
{
    *this = t.apply(*this);
    return *this;
}

const Vector3D Point3D::operator-(const Point3D & p) const
{
    return Vector3D(_x - p._x, _y - p._y, _z - p._z);
}

void Point3D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y << _z;
}

void Point3D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y >> _z;
}

void Point3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << "," << _z << ")";
}

void Point3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> "," >> _z >> ")";
}



//------------------------------------  Matrix33

const Matrix33 Matrix33::I = Matrix33(1, 0, 0, 0, 1, 0, 0, 0, 1);
const Matrix33 Matrix33::O = Matrix33(0, 0, 0, 0, 0, 0, 0, 0, 0);

const Matrix33 Matrix33::operator~() const
{
    return Matrix33(
                    +!minor11(), -!minor21(), +!minor31(),
                    -!minor12(), +!minor22(), -!minor32(),
                    +!minor13(), -!minor23(), +!minor33()
                    ) / (this->operator!());
}

const Matrix33 Matrix33::operator*(const Matrix33 & a) const
{
    return Matrix33(
                    (_m11 * a._m11) + (_m12 * a._m21) + (_m13 * a._m31),
                    (_m11 * a._m12) + (_m12 * a._m22) + (_m13 * a._m32),
                    (_m11 * a._m13) + (_m12 * a._m23) + (_m13 * a._m33),
                    (_m21 * a._m11) + (_m22 * a._m21) + (_m23 * a._m31),
                    (_m21 * a._m12) + (_m22 * a._m22) + (_m23 * a._m32),
                    (_m21 * a._m13) + (_m22 * a._m23) + (_m23 * a._m33),
                    (_m31 * a._m11) + (_m32 * a._m21) + (_m33 * a._m31),
                    (_m31 * a._m12) + (_m32 * a._m22) + (_m33 * a._m32),
                    (_m31 * a._m13) + (_m32 * a._m23) + (_m33 * a._m33)
                    );
}

const Matrix33 Matrix33::operator+(const Matrix33 & a) const
{
    return Matrix33(
                    _m11 + a._m11, _m12 + a._m12, _m13 + a._m13,
                    _m21 + a._m21, _m22 + a._m22, _m23 + a._m23,
                    _m31 + a._m31, _m32 + a._m32, _m33 + a._m33
                    );
}

Matrix33 & Matrix33::operator+=(const Matrix33 & a)
{
    _m11 += a._m11, _m12 += a._m12, _m13 += a._m13,
            _m21 += a._m21, _m22 += a._m22, _m23 += a._m23,
            _m31 += a._m31, _m32 += a._m32, _m33 += a._m33;

    return *this;
}

const Vector3D Matrix33::operator*(const Vector3D & v) const
{
    return Vector3D(
                    (_m11 * v.x()) + (_m12 * v.y()) + (_m13 * v.z()),
                    (_m21 * v.x()) + (_m22 * v.y()) + (_m23 * v.z()),
                    (_m31 * v.x()) + (_m32 * v.y()) + (_m33 * v.z())
                    );
}

/*
const Versor3D Matrix33::operator * (const Versor3D & v) const
{
        return Versor3D(*this * Vector3D(v));
}
 */


const Matrix33 Matrix33::operator-(const Matrix33 & m) const
{
    return *this + -m;
}

Matrix33 & Matrix33::operator-=(const Matrix33 & m)
{
    return *this += -m;
}

const Matrix33 Matrix33::rotation(double c, double s, const Versor3D & n)
{
    const double c1 = 1.0 - c;

    const double nx = n.x();
    const double ny = n.y();
    const double nz = n.z();

    return Matrix33(
                    c + nx * nx * c1, nx * ny * c1 - nz * s, nx * nz * c1 + ny * s,
                    ny * nx * c1 + nz * s, c + ny * ny * c1, ny * nz * c1 - nx * s,
                    nz * nx * c1 - ny * s, nz * ny * c1 + nx * s, c + nz * nz * c1
                    );
}

void Matrix33::_Serialization_serialize(BOStorage & s) const
{
    s << _m11 << _m12 << _m13 << _m21 << _m22 << _m23 << _m31 << _m32 << _m33;
}

void Matrix33::_Serialization_serialize(BIStorage & s)
{
    s >> _m11 >> _m12 >> _m13 >> _m21 >> _m22 >> _m23 >> _m31 >> _m32 >> _m33;
}

void Matrix33::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(";
    s << s.NL << _m11 << "," << _m12 << "," << _m13 << ",";
    s << s.NL << _m21 << "," << _m22 << "," << _m23 << ",";
    s << s.NL << _m31 << "," << _m32 << "," << _m33 << ")";
}

void Matrix33::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(";
    s >> _m11 >> "," >> _m12 >> "," >> _m13 >> ",";
    s >> _m21 >> "," >> _m22 >> "," >> _m23 >> ",";
    s >> _m31 >> "," >> _m32 >> "," >> _m33 >> ")";
}



//----------------------------  Versor3D

const Versor3D Versor3D::X = Versor3D(1, 0, 0);
const Versor3D Versor3D::Y = Versor3D(0, 1, 0);
const Versor3D Versor3D::Z = Versor3D(0, 0, 1);

void Versor3D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y << _z;
}

void Versor3D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y >> _z;
    normalize();
}

void Versor3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << "," << _z << ")";
}

void Versor3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> "," >> _z >> ")";
}


//----------------------------- Size3D

void Size3D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y;
}

void Size3D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y;
    absolutize();
}

void Size3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << ")";
}

void Size3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> ")";
    absolutize();
}


//---------------------------   Line3D

Line3D::Line3D(const Segment3D & s) : _p(s.a()), _u(s.b() - s.a())
{
}

Line3D::Line3D(const Point3D & p1, const Point3D & p2) : _p(p1), _u(p2 - p1)
{
}

Line3D::Line3D(const SemiLine3D & a) : _p(a.org()), _u(a.dir())
{
}

const Line3D Line3D::operator-(const Vector3D & k) const
{
    return *this + -k;
}

Line3D & Line3D::operator-=(const Vector3D & k)
{
    return *this += -k;
}

const Line3D Line3D::operator|(const Plane3D & k) const
{
    return Line3D(_p | k, Versor3D(Vector3D(_u) % k.nrm()));
}

Line3D & Line3D::operator|=(const Plane3D & k)
{
    _p |= k;
    _u = Versor3D(Vector3D(_u) % k.nrm());
    return *this;
}

const Line3D Line3D::operator+(const Vector3D & w) const
{
    return Line3D(_p + w, _u);
}

Line3D & Line3D::operator+=(const Vector3D & w)
{
    _p += w;
    return *this;
}

void Line3D::_Serialization_serialize(BOStorage & s) const
{
    s << _p << _u;
}

void Line3D::_Serialization_serialize(BIStorage & s)
{
    s >> _p >> _u;
}

void Line3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _p << "," << _u << ")";
}

void Line3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _p >> "," >> _u >> ")";
}



//----------------------------   Plane3D

Plane3D::Plane3D(const Triangle3D & t) : _p(t.a()), _n((t.b() - t.a()) ^ (t.c() - t.a()))
{
}

const Plane3D Plane3D::operator+(const Vector3D & w) const
{
    return Plane3D(_p + w, _n);
}

Plane3D & Plane3D::operator+=(const Vector3D & v)
{
    _p += v;
    return *this;
}

const Plane3D Plane3D::operator-(const Vector3D & v) const
{
    return Plane3D(_p - v, _n);
}

Plane3D & Plane3D::operator-=(const Vector3D & v)
{
    _p -= v;
    return *this;
}

void Plane3D::_Serialization_serialize(BOStorage & s) const
{
    s << _p << _n;
}

void Plane3D::_Serialization_serialize(BIStorage & s)
{
    s >> _p >> _n;
}

void Plane3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _p << "," << _n << ")";
}

void Plane3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _p >> "," >> _n >> ")";
}



//---------------------------   Segment3D

const Point3D Segment3D::middle() const
{
    return Point3D((_a.x() + _b.x()) / 2.0, (_a.y() + _b.y()) / 2.0, (_a.z() + _b.z()) / 2.0);
}

const Segment3D Segment3D::operator+(const Vector3D & v) const
{
    return Segment3D(_a + v, _b + v);
}

Segment3D & Segment3D::operator+=(const Vector3D & v)
{
    _a += v, _b += v;
    return *this;
}

const Segment3D Segment3D::operator-(const Vector3D & v) const
{
    return *this + -v;
}

Segment3D & Segment3D::operator-=(const Vector3D & v)
{
    return *this += -v;
}

const Segment3D Segment3D::operator|(const Line3D & r) const
{
    return Segment3D(_a | r, _b | r);
}

const Segment3D Segment3D::operator|(const Plane3D & k) const
{
    return Segment3D(_a | k, _b | k);
}

Segment3D & Segment3D::operator|=(const Line3D & r)
{
    _a |= r, _b |= r;
    return *this;
}

Segment3D & Segment3D::operator|=(const Plane3D & k)
{
    _a |= k, _b |= k;
    return *this;
}

void Segment3D::_Serialization_serialize(BOStorage & s) const
{
    s << _a << _b;
}

void Segment3D::_Serialization_serialize(BIStorage & s)
{
    s >> _a >> _b;
}

void Segment3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _a << "," << _b << ")";
}

void Segment3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _a >> "," >> _b >> ")";
}


//---------------------------------  Triangle3D

const Point3D Triangle3D::baricenter() const
{
    return Point3D((Vector3D(_a) + Vector3D(_b) + Vector3D(_c)) / 3.0);
}

const Vector3D Triangle3D::operator!() const
{
    return ((_b - _a) ^ (_c - _a)) / 2.0;
}

const Triangle3D Triangle3D::operator+(const Vector3D & v) const
{
    return Triangle3D(_a + v, _b + v, _c + v);
}

Triangle3D & Triangle3D::operator+=(const Vector3D & v)
{
    _a += v, _b += v, _c += v;
    return *this;
}

const Triangle3D Triangle3D::operator*(const Transform3D & tra) const
{
    Triangle3D t = *this;
    t *= tra;
    return t;
}

Triangle3D & Triangle3D::operator*=(const Transform3D & tra)
{
    _a *= tra;
    _b *= tra;
    _c *= tra;
    return *this;
}

const Triangle3D Triangle3D::operator-(const Vector3D & v) const
{
    return *this + -v;
}

Triangle3D & Triangle3D::operator-=(const Vector3D & v)
{
    return *this += -v;
}

const Triangle3D Triangle3D::operator|(const Plane3D & k) const
{
    return Triangle3D(_a | k, _b | k, _c | k);
}

Triangle3D & Triangle3D::operator|=(const Plane3D & k)
{
    _a |= k, _b |= k, _c |= k;
    return *this;
}

bool Triangle3D::coproj(const Point3D & p) const
{
    const Vector3D n = (_b - _a)^(_c - _a);
    const bool b0 = ((p - _a) * (n ^ (_a - _b)) >= 0);
    const bool b1 = ((p - _b) * (n ^ (_b - _c)) >= 0);
    const bool b2 = ((p - _c) * (n ^ (_c - _a)) >= 0);

    return (b0 && b1 && b2) || (!b0&&!b1&&!b2);
}

void Triangle3D::_Serialization_serialize(BOStorage &s) const
{
    s << _a << _b << _c;
}

void Triangle3D::_Serialization_serialize(BIStorage &s)
{
    s >> _a >> _b >> _c;
}

void Triangle3D::_Serialization_serialize(TOStorage &s) const
{
    s << classname() << "(" << _a << "," << _b << "," << _c << ")";
}

void Triangle3D::_Serialization_serialize(TIStorage &s)
{
    s >> classname() >> "(" >> _a >> "," >> _b >> "," >> _c >> ")";
}





//--------------------------- Tetrahedron

void Tetrahedron::_Serialization_serialize(BOStorage &s) const
{
    s << _a << _b << _c << _d;
}

void Tetrahedron::_Serialization_serialize(BIStorage &s)
{
    s >> _a >> _b >> _c >> _d;
}

void Tetrahedron::_Serialization_serialize(TOStorage &s) const
{
    s << classname() << "(" << _a << "," << _b << "," << _c << "," << _d << ")";
}

void Tetrahedron::_Serialization_serialize(TIStorage &s)
{
    s >> classname() >> "(" >> _a >> "," >> _b >> "," >> _c >> "," >> _d >> ")";
}

bool Solid3D::verify() const
{
    std::set<std::pair<t_ndx, t_ndx> > a;
    for (auto const & it : _faces)
    {
        if (!a.erase(std::make_pair(it._n2, it._n1))) a.insert(std::make_pair(it._n1, it._n2));
        if (!a.erase(std::make_pair(it._n3, it._n2))) a.insert(std::make_pair(it._n2, it._n3));
        if (!a.erase(std::make_pair(it._n1, it._n3))) a.insert(std::make_pair(it._n3, it._n1));
    }
    return a.empty();
}

void Solid3D::clear()
{
    invalidate();
    _faces.clear();
    _vertices.clear();
}

Solid3D::t_ndx Solid3D::addVertex(const Point3D & p)
{
    invalidate();
    _vertices.push_back(p);
    return _vertices.size() - 1;
}

void Solid3D::addFace(t_ndx n1, t_ndx n2, t_ndx n3)
{
    invalidate();
    ASSERT(n1 < _vertices.size());
    ASSERT(n2 < _vertices.size());
    ASSERT(n3 < _vertices.size());
    _faces.push_back(Face(n1, n2, n3));
}

const std::vector<Triangle3D> & Solid3D::faces() const
{
    computeFaces();
    return _cache_faces;
}

void Solid3D::computeFaces() const
{
    if (_cache_ok_faces) return;
    _cache_faces.clear();
    _cache_faces.reserve(_faces.size());
    for (auto const & it : _faces)
        _cache_faces.push_back(Triangle3D(_vertices[it._n1], _vertices[it._n2], _vertices[it._n3]));
    _cache_ok_faces = true;
}

const Solid3D Solid3D::operator+(const Vector3D & v) const
{
    Solid3D s;
    for (auto const & it : _vertices) s.addVertex(it + v);
    s._faces = _faces;
    return s;
}

Solid3D & Solid3D::operator+=(const Vector3D & v)
{
    invalidate();
    for (auto & it : _vertices) it += v;
    return *this;
}

const Solid3D Solid3D::operator-(const Vector3D & v) const
{
    return *this +(-v);
}

Solid3D & Solid3D::operator-=(const Vector3D & v)
{
    return *this += -v;
}

const Solid3D Solid3D::operator*(const Transform3D & tra) const
{
    Solid3D s;
    for (auto const & it : _vertices) s.addVertex(it * tra);
    s._faces = _faces;
    return s;
}

Solid3D & Solid3D::operator*=(const Transform3D & tra)
{
    invalidate();
    for (auto & it : _vertices) it *= tra;
    return *this;
}

const Point3D Solid3D::vcenter() const
{
    Vector3D v;
    for (auto const & it : _vertices) v += Vector3D(it);
    return Point3D(v / _vertices.size());
}

const Point3D Solid3D::baricenter() const
{
    Vector3D v;
    const Point3D c = vcenter();
    double vol = 0;
    const std::vector<Triangle3D> & f = faces();
    for (auto const & it : f)
    {
        Tetrahedron t(c, it.a(), it.b(), it.c());
        double tvol = t.volume();
        v += (t.baricenter() - c) * tvol;
        vol += tvol;
    }
    return c + v / vol;
}

double Solid3D::volume() const
{
    double v = 0;
    const Point3D p = vcenter();
    const std::vector<Triangle3D> & f = faces();
    for (auto const & it : f)
        v += !Matrix33(it.a() - p, it.b() - p, it.c() - p);
    return v / 6.0;
}

double Solid3D::area() const
{
    double a = 0;
    const std::vector<Triangle3D> & f = faces();
    for (auto const & it : f) a += !!it;
    return a;
}

bool Solid3D::co(const Point3D & p) const
{
    const std::vector<Triangle3D> & v = faces();
    double a = 0;
    for (auto const & it : v)
        a += solidAngle(Versor3D(p - it.a()), Versor3D(p - it.b()), Versor3D(p - it.c()));
    return a > 2 * M_PI;
}

void Solid3D::_Serialization_serialize(BOStorage &s) const
{
    s << _vertices << _faces;
}

void Solid3D::_Serialization_serialize(BIStorage &s)
{
    s >> _vertices >> _faces;
}

void Solid3D::_Serialization_serialize(TOStorage &s) const
{
    s << classname() << s.BEGIN;
    s << s.NL << "_vertices=" << _vertices;
    s << s.NL << "_faces=" << _faces;
    s << s.END;
}

void Solid3D::_Serialization_serialize(TIStorage &s)
{
    s >> classname() >> s.BEGIN;
    s >> "_vertices=" >> _vertices;
    s >> "_faces=" >> _faces;
    s >> s.END;
}

void Solid3D::Face::_Serialization_serialize(BOStorage & s) const
{
    s << _n1 << _n2 << _n3;
}

void Solid3D::Face::_Serialization_serialize(BIStorage & s)
{
    s >> _n1 >> _n2 >> _n3;
}

void Solid3D::Face::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _n1 << "," << _n2 << "," << _n3 << ")";
}

void Solid3D::Face::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _n1 >> "," >> _n2 >> "," >> _n3 >> ")";
}


//------------------------------------Transform3D

const Transform3D Transform3D::rotation(double alfa, const Versor3D & v, const Point3D & center)
{
    const Matrix33 mat = Matrix33::rotation(alfa, v);
    return Transform3D(mat, Vector3D(center) - mat * Vector3D(center));
}

const Transform3D Transform3D::rotation(const Versor3D & v1, const Versor3D & v2, const Point3D & center)
{
    const Vector3D v3 = Vector3D(v1) ^ Vector3D(v2);
    const double s = !v3;
    const double c = cos(v1, v2);
    const Matrix33 mat = Matrix33::rotation(c, s, Versor3D(v3));
    return Transform3D(mat, Vector3D(center) - mat * Vector3D(center));
}

const Transform3D Transform3D::scale(double kappa, const Point3D & center)
{
    const Matrix33 mat = Matrix33::I * kappa;
    return Transform3D(mat, Vector3D(center) - mat * Vector3D(center));
}

const Transform3D Transform3D::translation(const Vector3D & v)
{
    return Transform3D(Matrix33::I, v);
}

const Point3D Transform3D::apply(const Point3D & v) const
{
    return Point3D(_matrix * Vector3D(v) + _vector);
}

const Transform3D Transform3D::operator*(const Transform3D & tra) const
{
    return Transform3D(tra._matrix * _matrix, tra._matrix * _vector + tra._vector);
}

void Transform3D::_Serialization_serialize(BOStorage & s) const
{
    s << _matrix << _vector;
}

void Transform3D::_Serialization_serialize(BIStorage & s)
{
    s >> _matrix >> _vector;
}

void Transform3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _matrix << "," << _vector << ")";
}

void Transform3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _matrix >> "," >> _vector >> ")";
}



//------------------------------------------------ Polygon3D

const Point3D & Polygon3D::operator[](size_t n) const
{
    ASSERT(n < _p.size());
    return _p[n];
}

Point3D & Polygon3D::operator[](size_t n)
{
    ASSERT(n < _p.size());
    return _p[n];
}

Polygon3D Polygon3D::regular(size_t n)
{
    Polygon3D p(n);
    if (n)
    {
        double step = 2 * M_PI / n;

        LOOP(j, n)
        {
            double alfa = j * step;
            p._p[j] = Point3D(::cos(alfa), ::sin(alfa), 0.0);
        };
    };
    return p;
}

double Polygon3D::length() const
{
    double sum = 0;
    const size_t n = _p.size();
    LOOP(i, n) sum += dist(_p[i], _p[(i + 1) % n]);
    return sum;
}

Polygon3D & Polygon3D::operator+=(const Vector3D & shift)
{
    const size_t n = _p.size();
    LOOP(i, n)
    _p[i] += shift;
    return *this;
}

void Polygon3D::_Serialization_serialize(BOStorage & s) const
{
    s << _p;
}

void Polygon3D::_Serialization_serialize(BIStorage & s)
{
    s >> _p;
}

void Polygon3D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << s.BEGIN << _p << s.END;
}

void Polygon3D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> s.BEGIN >> _p >> s.END;
}

Polygon3D & Polygon3D::operator*=(const Transform3D & a)
{
    for (auto & it : _p) it *= a;
    return *this;
}

double dist2(const Segment3D & s1, const Segment3D & s2)
{
    const Line3D l1(s1);
    const Line3D l2(s2);

    const Versor3D n = Versor3D::ort(l1.dir(), l2.dir());
    const Versor3D n1 = Versor3D::ort(l1.dir(), n);
    const Versor3D n2 = Versor3D::ort(l2.dir(), n);

    const bool ja1 = (s1.a() - l2.org()) / n2 < 0;
    const bool jb1 = (s1.b() - l2.org()) / n2 < 0;

    const bool ja2 = (s2.a() - l1.org()) / n1 < 0;
    const bool jb2 = (s2.b() - l1.org()) / n1 < 0;

    const bool j1 = (ja1 != jb1);
    const bool j2 = (ja2 != jb2);

    if (j1 && j2) return sqr((l1.org() - l2.org()) / n);

    Minimal<double> m;
    if (j1)
    {
        m |= dist2(s2.a(), s1);
        m |= dist2(s2.b(), s1);
    }

    if (j2)
    {
        m |= dist2(s1.a(), s2);
        m |= dist2(s1.b(), s2);
    }

    if (!j1 && !j2)
    {
        m |= dist2(s1.a(), s2.a());
        m |= dist2(s1.a(), s2.b());
        m |= dist2(s1.b(), s2.a());
        m |= dist2(s1.b(), s2.b());
    }

    return m;
}

double dist2(const Triangle3D & t1, const Triangle3D & t2)
{
    Minimal<double> m;
    m |= dist2(Segment3D(t1.a(), t1.b()), t2);
    m |= dist2(Segment3D(t1.b(), t1.c()), t2);
    m |= dist2(Segment3D(t1.c(), t1.a()), t2);
    m |= dist2(Segment3D(t2.a(), t2.b()), t1);
    m |= dist2(Segment3D(t2.b(), t2.c()), t1);
    m |= dist2(Segment3D(t2.c(), t2.a()), t1);

    return m;
}

double dist2(const Point3D & p, const Segment3D & s)
{
    //if(s.length2()==0.0) return dist2(p,s.a());
    const Line3D w(s);
    const Point3D q = p | w;
    if (s.coproj(p)) return dist2(p, q);
    return std::min(dist2(p, s.a()), dist2(p, s.b()));
}

double dist2(const Point3D & p, const Triangle3D & t)
{
    //if((t.a()-t.b()).qdr()==0) return dist2(p,Segment3D(t.a(),t.c()));
    //if((t.b()-t.c()).qdr()==0) return dist2(p,Segment3D(t.b(),t.a()));
    //if((t.c()-t.a()).qdr()==0) return dist2(p,Segment3D(t.c(),t.b()));

    const Point3D q = p | Plane3D(t);
    if (t.coproj(p)) return dist2(p, q);

    Minimal<double> m;
    m |= dist2(p, Segment3D(t.a(), t.b()));
    m |= dist2(p, Segment3D(t.b(), t.c()));
    m |= dist2(p, Segment3D(t.c(), t.a()));
    return m;
}

double dist2(const Segment3D & s, const Triangle3D & t)
{
    if (collide(s, t)) return 0;

    Minimal<double> m;
    m |= dist2(s.a(), t);
    m |= dist2(s.b(), t);
    m |= dist2(Segment3D(t.a(), t.b()), s);
    m |= dist2(Segment3D(t.b(), t.c()), s);
    m |= dist2(Segment3D(t.c(), t.a()), s);

    return m;
}

Point3D inters(const Line3D & r, const Plane3D & alfa)
{
    const double den = cos(alfa.nrm(), r.dir());
    const double num = (alfa.org() - r.org()) / alfa.nrm();
    return r.org() + r.dir() * (num / den);
}

Point3D inters(const Segment3D & s, const Triangle3D & t)
{
    const Point3D p = inters(Line3D(s), Plane3D(t));
    return (s.coproj(p) && t.coproj(p)) ? p : Point3D::NaN;
}

bool collide(const Segment3D & s, const Triangle3D & t)
{
    const Point3D p = inters(Line3D(s), Plane3D(t));
    return (s.coproj(p) && t.coproj(p));
}

Point3D inters(const SemiLine3D & r, const Triangle3D & t)
{
    const Point3D p = inters(Line3D(r), Plane3D(t));
    return (r.coproj(p) && t.coproj(p)) ? p : Point3D::NaN;
}

double solidAngle(const Versor3D & v1, const Versor3D & v2, const Versor3D & v3)
{
    const Versor3D w1 = Versor3D::ort(v1, v2);
    const Versor3D w2 = Versor3D::ort(v2, v3);
    const Versor3D w3 = Versor3D::ort(v3, v1);

    const double a1 = acos(cos(w1, -w2));
    const double a2 = acos(cos(w2, -w3));
    const double a3 = acos(cos(w3, -w1));

    return a1 + a2 + a3 - M_PI;
}


CXM_NS_END
