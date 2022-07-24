// YAL zeldan

#define _USE_MATH_DEFINES

#include <float.h>

#include <cmath>
#include <algorithm>
#include <limits>

#include <cxu/cxu.hxx>

#include "linear2d.hxx"
#include "real.hxx"
#include "stats.hxx"

#include <cxu/mymacros.hxx>
#include "mymacros.hxx"

using namespace std;
using namespace cxu;


CXM_NS_BEGIN

        //------------- Vector2D ---------------------------------

        const Vector2D Vector2D::O = Vector2D(0, 0);

Vector2D::Vector2D(const Point2D & p) : _x(p.x()), _y(p.y())
{
}

Vector2D::Vector2D(const Size2D & a) : _x(a.x()), _y(a.y())
{
}

Vector2D::Vector2D(const Versor2D & v) : _x(v.x()), _y(v.y())
{
}

Vector2D & Vector2D::operator+=(const Vector2D & v)
{
    _x += v._x;
    _y += v._y;
    return *this;
}

Vector2D & Vector2D::operator-=(const Vector2D & v)
{
    _x -= v._x;
    _y -= v._y;
    return *this;
}

double Vector2D::operator/(const Versor2D & v) const
{
    return _x * v.x() + _y * v.y();
}

const Vector2D Vector2D::operator|(const Versor2D & u) const
{
    return u * (*this / u);
}

Vector2D & Vector2D::operator%=(const Versor2D & v)
{
    *this -= (*this | v);
    return *this;
}

void Vector2D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y;
}

void Vector2D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y;
}

void Vector2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << ")";
}

void Vector2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> ")";
}




//--------------------------- Point2D


const Point2D Point2D::NaN = Point2D(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());

Point2D & Point2D::operator+=(const Vector2D & v)
{
    _x += v.x(), _y += v.y();
    return *this;
}

Point2D & Point2D::operator-=(const Vector2D & v)
{
    _x -= v.x(), _y -= v.y();
    return *this;
}

const Point2D Point2D::operator*(const Transform2D & t) const
{
    return t.apply(*this);
}

Point2D & Point2D::operator*=(const Transform2D & t)
{
    *this = t.apply(*this);
    return *this;
}

const Vector2D Point2D::operator-(const Point2D & p) const
{
    return Vector2D(_x - p._x, _y - p._y);
}

const Point2D Point2D::operator|(const Line2D & r) const
{
    return r.org() + ((*this -r.org()) | r.dir());
}

void Point2D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y;
}

void Point2D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y;
}

void Point2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << ")";
}

void Point2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> ")";
}




//----------------------- Versor2D


const Versor2D Versor2D::X = Versor2D(1, 0);
const Versor2D Versor2D::Y = Versor2D(0, 1);

void Versor2D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y;
}

void Versor2D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y;
    normalize();
}

void Versor2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << ")";
}

void Versor2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> ")";
    normalize();
}


//------------------------------ Size2D

void Size2D::_Serialization_serialize(BOStorage & s) const
{
    s << _x << _y;
}

void Size2D::_Serialization_serialize(BIStorage & s)
{
    s >> _x >> _y;
    absolutize();
}

void Size2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _x << "," << _y << ")";
}

void Size2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _x >> "," >> _y >> ")";
    absolutize();
}


//------------------------------------ Matrix22


const Matrix22 Matrix22::I = Matrix22(1, 0, 0, 1);
const Matrix22 Matrix22::O = Matrix22(0, 0, 0, 0);

const Matrix22 Matrix22::rotation(double theta)
{
    const double cosTheta = ::cos(theta);
    const double sinTheta = ::sin(theta);
    return Matrix22(cosTheta, -sinTheta,
                    sinTheta, cosTheta);
}

const Matrix22 Matrix22::rotation(const Versor2D & v)
{
    const double cosTheta = v.x();
    const double sinTheta = v.y();
    return Matrix22(cosTheta, -sinTheta,
                    sinTheta, cosTheta);
}

void Matrix22::_Serialization_serialize(BOStorage & s) const
{
    s << _m11 << _m12 << _m21 << _m22;
}

void Matrix22::_Serialization_serialize(BIStorage & s)
{
    s >> _m11 >> _m12 >> _m21 >> _m22;
}

void Matrix22::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _m11 << "," << _m12 << "," << _m21 << "," << _m22 << ")";
}

void Matrix22::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _m11 >> "," >> _m12 >> "," >> _m21 >> "," >> _m22 >> ")";
}


//----------------------- Line2D

Line2D::Line2D(const SemiLine2D & a) : _p(a.org()), _u(a.dir())
{
}

const Line2D Line2D::operator*(const Transform2D & t) const
{
    return Line2D(_p*t, Versor2D(t.matrix() * Vector2D(_u)));
}

Line2D & Line2D::operator*=(const Transform2D & t)
{
    _p *= t;
    _u = Versor2D(t.matrix() * Vector2D(_u));
    return *this;
}

void Line2D::_Serialization_serialize(BOStorage & s) const
{
    s << _p << _u;
}

void Line2D::_Serialization_serialize(BIStorage & s)
{
    s >> _p >> _u;
}

void Line2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _p << "," << _u << ")";
}

void Line2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _p >> "," >> _u >> ")";
}

//----------------------------- SemiLine2D

const SemiLine2D SemiLine2D::operator*(const Transform2D & t) const
{
    return SemiLine2D(_p*t, Versor2D(t.matrix() * Vector2D(_u)));
}

void SemiLine2D::_Serialization_serialize(BOStorage & s) const
{
    s << _p << _u;
}

void SemiLine2D::_Serialization_serialize(BIStorage & s)
{
    s >> _p >> _u;
}

void SemiLine2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _p << "," << _u << ")";
}

void SemiLine2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _p >> "," >> _u >> ")";
}


//---------------------------------- Segment2D

/**
        DESCRIPTION
        Asse
 */
const Line2D Segment2D::axis() const
{
    return Line2D(Point2D((Vector2D(_a) + Vector2D(_b)) / 2), Versor2D((_a - _b).ort()));
}

Segment2D& Segment2D::operator+=(const Vector2D & v)
{
    _a += v;
    _b += v;
    return *this;
}

const Segment2D Segment2D::operator*(const Transform2D & tra) const
{
    return Segment2D(_a*tra, _b * tra);
}

void Segment2D::_Serialization_serialize(BOStorage & s) const
{
    s << _a << _b;
}

void Segment2D::_Serialization_serialize(BIStorage & s)
{
    s >> _a >> _b;
}

void Segment2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _a << "," << _b << ")";
}

void Segment2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _a >> "," >> _b >> ")";
}




//-------------------------------------- Polygon2D

Polygon2D Polygon2D::regular(size_t n)
{
    Polygon2D p(n);
    if (n)
    {
        const double step = 2 * M_PI / n;

        LOOP(j, n)
        {
            double alfa = j * step;
            p._p[j] = Point2D(::cos(alfa), ::sin(alfa));
        };
    };

    return p;
}

const Point2D Polygon2D::vcenter() const
{
    Vector2D sum;
    for (auto const & it : _p) sum += Vector2D(it);
    return Point2D(sum / double(_p.size()));
}

double Polygon2D::area() const
{
    Point2D c = vcenter();
    const size_t n = _p.size();
    Vector2D v1, v2;
    double a = 0;

    LOOP(j, n)
    {
        v1 = _p[j] - c;
        v2 = _p[(j + 1) % n] - c;
        a += v1^v2;
    };

    return a / 2;
}

double Polygon2D::length() const
{
    double sum = 0;
    const size_t n = _p.size();
    LOOP(j, n) sum += dist(_p[j], _p[(j + 1) % n]);
    return sum;
}

// Ritorna la lunghezza di un segmento

double Polygon2D::length(size_t j) const
{
    const size_t n = _p.size();
    ASSERT(j < n);
    return dist(_p[j], _p[(j + 1) % n]);
}

int Polygon2D::index(const Point2D & p) const
{
    double k = 0;
    const size_t n = _p.size();
    Vector2D v1, v2;

    LOOP(j, n)
    {
        v1 = _p[j] - p;
        v2 = _p[(j + 1) % n] - p;
        k += atan2(v1^v2, v1 * v2);
    };

    return int(k / (2 * M_PI) + 0.5);
}

Polygon2D & Polygon2D::operator+=(const Vector2D & shift)
{
    for (size_t i = 0; i < _p.size(); ++i) _p[i] += shift;
    return *this;
}

Polygon2D Polygon2D::operator+(const Vector2D & shift) const
{
    Polygon2D polygon(*this);
    polygon += shift;
    return polygon;
}

Polygon2D & Polygon2D::operator*=(const Transform2D & tr)
{
    const size_t n = _p.size();
    LOOP(i, n) _p[i] *= tr;
    return *this;
}

Polygon2D Polygon2D::operator*(const Transform2D & tr) const
{
    Polygon2D polygon(*this);
    const size_t n = polygon._p.size();
    LOOP(i, n)
    polygon._p[i] *= tr;
    return polygon;
}

const Point2D Polygon2D::at(double t) const
{
    const size_t z = _p.size();
    ASSERT(z >= 2);
    double len = 0;
    size_t i = 0;

    for (;;)
    {
        const double k = length(i % z);
        const double kl = len + k;
        if (kl > t) break;
        len = kl;
        ++i;
    }

    return _p[i % z] + Versor2D(_p[(i + 1) % z] - _p[i % z]) * (t - len);
}

void Polygon2D::_Serialization_serialize(BOStorage & s) const
{
    s << _p;
}

void Polygon2D::_Serialization_serialize(BIStorage & s)
{
    s >> _p;
}

void Polygon2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << s.BEGIN << _p << s.END;
}

void Polygon2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> s.BEGIN >> _p >> s.END;
}





//---------------------------------------- Transform2D

const Transform2D Transform2D::operator~() const
{
    const Matrix22 mat = ~_matrix;
    return Transform2D(mat, -mat * _vector);
}

/**
        DESCRIPTION
        Compone due trasformazioni nell'ordine indicato
        NOTE
        L'ordine e' inverso al normale prodotto di matrici.
        EXAMPLE
        Point2D p,q;
        Trasnform2D h,k;
        q = p * (h,k);
 */
const Transform2D Transform2D::operator*(const Transform2D & tra) const
{
    return Transform2D(tra._matrix * _matrix, tra._matrix * _vector + tra._vector);
}

Transform2D & Transform2D::operator*=(const Transform2D & tra)
{
    _vector = tra._matrix * _vector + tra._vector;
    _matrix = tra._matrix * _matrix;
    return *this;
}

/**
        DESCRIPTION
        Applica la trasformazione a un punto
 */
const Point2D Transform2D::apply(const Point2D & v) const
{
    return Point2D(_matrix * Vector2D(v)) + _vector;
}

const Transform2D Transform2D::translation(const Vector2D & v)
{
    return Transform2D(Matrix22::I, v);
}

const Transform2D Transform2D::rotation(double alfa, const Point2D & center)
{
    const Matrix22 mat = Matrix22::rotation(alfa);
    return Transform2D(mat, Vector2D(center) - mat * Vector2D(center));
}

const Transform2D Transform2D::rotation(const Versor2D & dir, const Point2D & center)
{
    const Matrix22 mat = Matrix22(dir.x(), -dir.y(), dir.y(), dir.x());
    return Transform2D(mat, Vector2D(center) - mat * Vector2D(center));
}

const Transform2D Transform2D::rotation(double alfa)
{
    return Transform2D(Matrix22::rotation(alfa), Vector2D::O);
}

const Transform2D Transform2D::rotation(const Versor2D & dir)
{
    return Transform2D(Matrix22(dir.x(), -dir.y(), dir.y(), dir.x()), Vector2D::O);
}

const Transform2D Transform2D::scale(const Vector2D & k)
{
    return Transform2D(Matrix22(k.x(), 0, 0, k.y()), Vector2D::O);
}

void Transform2D::_Serialization_serialize(BOStorage & s) const
{
    s << _matrix << _vector;
}

void Transform2D::_Serialization_serialize(BIStorage & s)
{
    s >> _matrix >> _vector;
}

void Transform2D::_Serialization_serialize(TOStorage & s) const
{
    s << classname() << "(" << _matrix << "," << _vector << ")";
}

void Transform2D::_Serialization_serialize(TIStorage & s)
{
    s >> classname() >> "(" >> _matrix >> "," >> _vector >> ")";
}



//----------------------------------------- globals

double angle(const Versor2D & v1, const Versor2D & v2)
{
    const double k = acos(cos(v1, v2));
    return (sin(v2, v1) < 0) ? -k : k;
}

Point2D inters(const Segment2D & s1, const Segment2D & s2)
{
    const Line2D r1(s1);
    const Line2D r2(s2);
    const Point2D p = inters(r1, r2);
    return (s1.coproj(p) && s2.coproj(p)) ? p : Point2D::NaN;
}

bool collide(const Segment2D & s1, const Segment2D & s2)
{
    const Line2D r1(s1);
    const Line2D r2(s2);
    const Point2D p = inters(r1, r2);
    return (s1.coproj(p) && s2.coproj(p));
}

Point2D inters(const Line2D & r1, const Line2D & r2)
{
    const double d = !(Matrix22(Vector2D(r1._u), -Vector2D(r2._u)));
    const double h = !(Matrix22(r2._p - r1._p, -Vector2D(r2._u))) / d;
    const double k = !(Matrix22(Vector2D(r1._u), r2._p - r1._p)) / d;
    return Point2D(((Vector2D(r1._p) + r1._u * h) + (Vector2D(r2._p) + r2._u * k)) / 2);
}

Point2D inters(const Segment2D & s, const SemiLine2D & r)
{
    const Point2D p = inters(Line2D(r), Line2D(s));
    const double q = (p - r.org()) / r.dir();
    return (s.coproj(p) && (q >= 0)) ? p : Point2D::NaN;
}

bool collide(const Segment2D & s, const SemiLine2D & r)
{
    const Point2D p = inters(Line2D(r), Line2D(s));
    const double q = (p - r.org()) / r.dir();
    return (s.coproj(p) && (q >= 0));
}

Point2D inters(const Segment2D & s, const Line2D & r)
{
    const Point2D p = inters(Line2D(r), Line2D(s));
    return (s.coproj(p)) ? p : Point2D::NaN;
}

double dist2(const Point2D & p, const Segment2D & s)
{
    if (s.coproj(p)) return dist2(p, p | Line2D(s));
    else return std::min(dist2(p, s.a()), dist2(p, s.b()));
}

double dist2(const Segment2D & s1, const Segment2D & s2)
{
    Minimal<double> m;
    m |= dist2(s1.a(), s2);
    m |= dist2(s1.b(), s2);
    m |= dist2(s2.a(), s1);
    m |= dist2(s2.b(), s1);

    return m;
}

double dist2(const Polygon2D & a, const Polygon2D & b)
{

    const std::vector<Point2D> va = a.points();
    const std::vector<Point2D> vb = b.points();

    const size_t na = va.size();
    const size_t nb = vb.size();

    double d = std::numeric_limits<double>::max();

    LOOP(ia, na) LOOP(ib, nb)
    {
        Segment2D sa(va[ia], va[(ia + 1) % na]);
        Segment2D sb(vb[ib], vb[(ib + 1) % nb]);

        const double q1 = dist2(va[ia], sb);
        if (q1 <= 0.0)
            return 0.0;

        const double q2 = dist2(vb[ib], sa);
        if (q2 <= 0.0)
            return 0.0;

        d = std::min(dist, q1);
        d = std::min(dist, q2);
    }

    return d;
}

bool collide(const Polygon2D & a, const Polygon2D & b)
{
    const std::vector<Point2D> va = a.points();
    const std::vector<Point2D> vb = b.points();

    const size_t na = va.size();
    const size_t nb = vb.size();

    if (na == 0 || nb == 0) return false;

    LOOP(ia, na) LOOP(ib, nb)
    {
        const Segment2D sa(va[ia], va[(ia + 1) % na]);
        const Segment2D sb(vb[ib], vb[(ib + 1) % nb]);
        if (collide(sa, sb)) return true;
    };

    if (a.index(vb[0]) != 0) return true;
    if (b.index(va[0]) != 0) return true;

    return false;
}



//-------------ConvexHullAlgorithm--------------

class ConvexHull_Sort : public std::binary_function<const std::pair<Point2D, size_t>&, const std::pair<Point2D, size_t>&, bool>
{
public:

    explicit ConvexHull_Sort(const Point2D & p0)
    : _p0(p0)
    {
    }

    bool operator()(const std::pair<Point2D, size_t> & a, const std::pair<Point2D, size_t> & b)
    {
        const double w = sin(a.first - _p0, b.first - _p0);
        if (w > 0) return true;
        if (w < 0) return false;
        return dist(a.first, _p0) < dist(b.first, _p0);
    }

protected:
    Point2D _p0;

};

std::vector<Point2D> convexHull(const std::vector<Point2D> & in, std::set<size_t> * used)
{
    std::vector<Point2D> out;
    if (used) used->clear();

    if (in.size() < 3)
    {
        if (used)
            LOOP(j, in.size())
            used->insert(j);
        out = in;
        return out;
    }

    std::vector<std::pair<Point2D, size_t> > ink;
    {
        const size_t z = in.size();
        LOOP(j, z) ink.push_back(std::make_pair(in[j], j));
    }

    std::vector<std::pair<Point2D, size_t> > outk;

    Point2D p0;
    {
        Stat stat(Stat::MINMAX);
        for (auto const & it : ink) stat.add(it.first.y());
        const size_t nm = stat.nMin();
        outk.push_back(ink[nm]);
        p0 = ink[nm].first;
        ink.erase(ink.begin() + nm);
    }


    ConvexHull_Sort mysort(p0);
    std::sort(ink.begin(), ink.end(), mysort);

    {
        std::vector<std::pair<Point2D, size_t> >::const_iterator it = ink.begin();
        outk.push_back(*it);
        ++it;
        outk.push_back(*it);
        ++it;

        for (; it != ink.end(); ++it)
        {
            const Point2D pi = it->first;
            for (;;)
            {
                if (outk.size() < 2) break;
                const Point2D ptop = outk.rbegin()->first;
                const Point2D pprv = (outk.rbegin() + 1)->first;
                const double k = sin(pprv - ptop, pi - ptop);
                if (k < 0) break;
                else outk.pop_back();
            };
            outk.push_back(*it);
        }
    }

    out.clear();
    for (auto const & it : outk)
    {
        if (used)
            used->insert(it.second);
        out.push_back(it.first);
    }

    return out;
}

CXM_NS_END
