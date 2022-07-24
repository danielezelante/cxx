// YAL zeldan

#define _USE_MATH_DEFINES
#include <cmath>

#include <cxu/cxu.hxx>

#include "types.hxx"
#include "conic2d.hxx"

using namespace cxu;

CXM_NS_BEGIN

using ::sqrt;

//---------------- static

static bool arc2D_periodic_include(double x, double a, double b)
{
    double xm = angle_Pi_Pi(x);
    const double am = angle_Pi_Pi(a);
    double bm = angle_Pi_Pi(b);

    if (am > bm) bm += 2 * M_PI;
    if (am > xm) xm += 2 * M_PI;

    return inco(xm, am, bm);
}


//--------------------------- Circle2D

const Circle2D Circle2D::U = Circle2D(Point2D(0, 0), 1);

void Circle2D::_Serialization_serialize(BOStorage & s) const
{
    s << _p << _r;
}

void Circle2D::_Serialization_serialize(BIStorage & s)
{
    s >> _p >> _r;
}

void Circle2D::_Serialization_serialize(TOStorage & s) const
{
    s << "Circle2D" << "(" << _p << "," << _r << ")";
}

void Circle2D::_Serialization_serialize(TIStorage & s)
{
    s >> "Circle2D" >> "(" >> _p >> "," >> _r >> ")";
}

std::pair<Point2D, Point2D> inters(const Circle2D & ce, const Line2D & rt)
{
    const Vector2D o1 = rt.org() - ce.center();
    const double b = (o1 / rt.dir()) * 2;
    const double c = o1.qdr() - sqr(ce.radius());

    std::pair<double, double> sol = solve2G(1.0, b, c);

    return std::make_pair(rt.org() + rt.dir() * sol.first, rt.org() + rt.dir() * sol.second);
}

std::pair<Point2D, Point2D> inters(const Circle2D & ce, const SemiLine2D & rt)
{
    const Vector2D o1 = rt.org() - ce.center();
    const double b = (o1 / rt.dir()) * 2;
    const double c = o1.qdr() - sqr(ce.radius());

    std::pair<double, double> sol = solve2G(1.0, b, c);
    Point2D p1 = Point2D::NaN;
    Point2D p2 = Point2D::NaN;
    if (sol.first >= 0) p1 = rt.org() + rt.dir() * sol.first;
    if (sol.second >= 0) p2 = rt.org() + rt.dir() * sol.second;
    if (!p1.isFinite() && p2.isFinite()) xchg(p1, p2);
    return std::make_pair(p1, p2);
}

std::pair<Point2D, Point2D> inters(const Circle2D & c, const Segment2D & r)
{
    std::pair<Point2D, Point2D> pt = inters(c, Line2D(r));
    Point2D & p1 = pt.first;
    Point2D & p2 = pt.second;

    if (!r.coproj(p1)) p1 = Point2D::NaN;
    if (!r.coproj(p2)) p2 = Point2D::NaN;
    if (!p1.isFinite() && p2.isFinite()) xchg(p1, p2);

    return pt;
}



//----------------- Arc2D

const Point2D Arc2D::pointA() const
{
    return _c.center() + Versor2D(_a) * _c.radius();
}

const Point2D Arc2D::pointB() const
{
    return _c.center() + Versor2D(_b) * _c.radius();
}

void Arc2D::_Serialization_serialize(BOStorage & s) const
{
    s << _c << _a << _b;
}

void Arc2D::_Serialization_serialize(BIStorage & s)
{
    s >> _c >> _a >> _b;
}

void Arc2D::_Serialization_serialize(TOStorage & s) const
{
    s << "Arc2D" << "(" << _c << "," << _a << "," << _b << ")";
}

void Arc2D::_Serialization_serialize(TIStorage & s)
{
    s >> "Arc2D" >> "(" >> _c >> "," >> _a >> "," >> _b >> ")";
}

std::pair<Point2D, Point2D> inters(const Arc2D & arc, const Line2D & ret)
{
    std::pair<Point2D, Point2D> pt = inters(arc.circle(), ret);
    Point2D & h1 = pt.first;
    Point2D & h2 = pt.second;

    if (!(arc2D_periodic_include(~(h1 - arc.circle().center()), arc.a(), arc.b())))
        h1 = Point2D::NaN;

    if (!(arc2D_periodic_include(~(h2 - arc.circle().center()), arc.a(), arc.b())))
        h2 = Point2D::NaN;

    if (!h1.isFinite() && h2.isFinite()) xchg(h1, h2);
    return pt;
}

std::pair<Point2D, Point2D> inters(const Arc2D & arc, const SemiLine2D & ret)
{
    std::pair<Point2D, Point2D> pt = inters(arc.circle(), Line2D(ret));
    Point2D & h1 = pt.first;
    Point2D & h2 = pt.second;

    if (!(arc2D_periodic_include(~(h1 - arc.circle().center()), arc.a(), arc.b()) && ret.coproj(h1)))
        h1 = Point2D::NaN;

    if (!(arc2D_periodic_include(~(h2 - arc.circle().center()), arc.a(), arc.b()) && ret.coproj(h2)))
        h2 = Point2D::NaN;

    if (!h1.isFinite() && h2.isFinite()) xchg(h1, h2);
    return pt;
}

std::pair<Point2D, Point2D> inters(const Arc2D & arc, const Segment2D & seg)
{
    std::pair<Point2D, Point2D> pt = inters(arc.circle(), seg);
    Point2D & h1 = pt.first;
    Point2D & h2 = pt.second;

    if (!(arc2D_periodic_include(~(h1 - arc.circle().center()), arc.a(), arc.b()) && seg.coproj(h1)))
        h1 = Point2D::NaN;

    if (!(arc2D_periodic_include(~(h2 - arc.circle().center()), arc.a(), arc.b()) && seg.coproj(h2)))
        h2 = Point2D::NaN;

    if (!h1.isFinite() && h2.isFinite()) xchg(h1, h2);
    return pt;
}



//------------------- Ellipse2D

Ellipse2D & Ellipse2D::operator*=(const Transform2D & tr)
{
    _t *= tr;
    return *this;
}

const Vector2D Ellipse2D::axis() const
{
    const Matrix22 mat = ~(_t.matrix());
    const double ax2 = mat.c1().qdr();
    const double ay2 = mat.c2().qdr();
    const double axy = mat.c1() * mat.c2();
    const double sqdelta = sqrt(sqr(ax2 - ay2) + 4 * sqr(axy));
    const double lambda1 = (ax2 + ay2 + sqdelta) / 2;
    const double lambda2 = (ax2 + ay2 - sqdelta) / 2;

    const double w = 1.0 / sqrt(lambda2);
    const double s1 = fabs(lambda1 - std::min(ax2, ay2));
    const double s2 = fabs(axy);

    double z1, z2;
    if (s1 < s2) z1 = s1, z2 = s2;
    else z1 = s2, z2 = s1;

    //printf("# ax2=%g, ay2=%g, axy=%g, sqdelta=%g, lambda1=%g, lambda2=%g, z1=%g, z2=%g\n",ax2,ay2,axy,sqdelta,lambda1,lambda2,z1,z2);

    if (ax2 >= ay2)
    {
        if (axy >= 0)
            return Versor2D(z1, -z2) * w;
        else
            return Versor2D(z1, +z2) * w;
    }
    else
    {
        if (axy >= 0)
            return Versor2D(z2, -z1) * w;
        else
            return Versor2D(z2, +z1) * w;
    }
}

double Ellipse2D::ratio() const
{
    const Matrix22 mat = ~(_t.matrix());
    const double ax2 = mat.c1().qdr();
    const double ay2 = mat.c2().qdr();
    const double axy = mat.c1() * mat.c2();
    const double sqdelta = sqrt(sqr(ax2 - ay2) + 4 * sqr(axy));
    const double dlambda1 = ax2 + ay2 + sqdelta;
    const double dlambda2 = ax2 + ay2 - sqdelta;
    return sqrt(dlambda2 / dlambda1);
}

void Ellipse2D::_Serialization_serialize(BOStorage & s) const
{
    s << _t;
}

void Ellipse2D::_Serialization_serialize(BIStorage & s)
{
    s >> _t;
}

void Ellipse2D::_Serialization_serialize(TOStorage & s) const
{
    s << "Ellipse2D" << "(" << _t << ")";
}

void Ellipse2D::_Serialization_serialize(TIStorage & s)
{
    s >> "Ellipse2D" >> "(" >> _t >> ")";
}

std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const Line2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Circle2D::U, r*~c.t());
    return std::make_pair(w.first * c.t(), w.second * c.t());
}

std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const SemiLine2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Circle2D::U, r*~c.t());
    return std::make_pair(w.first * c.t(), w.second * c.t());
}

std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const Segment2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Circle2D::U, r*~c.t());
    return std::make_pair(w.first * c.t(), w.second * c.t());
}


//------------ EllipticArc2D

EllipticArc2D & EllipticArc2D::operator*=(const Transform2D & t)
{
    _e *= t;
    return *this;
}

void EllipticArc2D::_Serialization_serialize(BOStorage & s) const
{
    s << _e << _a << _b;
}

void EllipticArc2D::_Serialization_serialize(BIStorage & s)
{
    s >> _e >> _a >> _b;
}

void EllipticArc2D::_Serialization_serialize(TOStorage & s) const
{
    s << "EllipticArc2D" << "(" << _e << "," << _a << "," << _b << ")";
}

void EllipticArc2D::_Serialization_serialize(TIStorage & s)
{
    s >> "EllipticArc2D" >> "(" >> _e >> "," >> _a >> "," >> _b >> ")";
}

std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const Line2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Arc2D(Circle2D::U, c.a(), c.b()), r*~c.ellipse().t());
    return std::make_pair(w.first * c.ellipse().t(), w.second * c.ellipse().t());
}

std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const SemiLine2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Arc2D(Circle2D::U, c.a(), c.b()), r*~c.ellipse().t());
    return std::make_pair(w.first * c.ellipse().t(), w.second * c.ellipse().t());
}

std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const Segment2D & r)
{
    std::pair<Point2D, Point2D> w = inters(Arc2D(Circle2D::U, c.a(), c.b()), r*~c.ellipse().t());
    return std::make_pair(w.first * c.ellipse().t(), w.second * c.ellipse().t());
}




//--------------- global

std::pair<Point2D, Point2D> inters(const Circle2D & c1, const Circle2D & c2)
{
    const double r1 = c1.radius();
    const double r2 = c2.radius();
    const Point2D & q1 = c1.center();
    const Point2D & q2 = c2.center();
    const double d = dist(q1, q2);
    const double a = d / 2 + (sqr(r1) - sqr(r2)) / (d * 2);
    const double kq = sqr(r1) - sqr(a);
    if (kq < 0) return std::make_pair(Point2D::NaN, Point2D::NaN); // for optimization only
    const double k = sqrt(kq);

    const Vector2D v(Versor2D(q2 - q1) * a);
    const Vector2D w(Versor2D(v.ort()) * k);

    return std::make_pair(q1 + v + w, q1 + v - w);
}

std::pair<Point2D, Point2D> inters(const Arc2D & a1, const Arc2D & a2)
{
    std::pair<Point2D, Point2D> pt = inters(a1.circle(), a2.circle());
    Point2D & h1 = pt.first;
    Point2D & h2 = pt.second;

    if (
            !arc2D_periodic_include(~(h1 - a1.circle().center()), a1.a(), a1.b()) ||
            !arc2D_periodic_include(~(h1 - a2.circle().center()), a2.a(), a2.b())
            )
        h1 = Point2D::NaN;

    if (
            !arc2D_periodic_include(~(h2 - a1.circle().center()), a1.a(), a1.b()) ||
            !arc2D_periodic_include(~(h2 - a2.circle().center()), a2.a(), a2.b())
            )
        h2 = Point2D::NaN;

    if (!h1.isFinite() && h2.isFinite()) xchg(h1, h2);
    return pt;
}

double dist2(const Point2D & p, const Circle2D & c)
{
    const Point2D i = c.center() + Versor2D(p - c.center()) * c.radius();
    return dist2(p, i);
}

double dist2(const Point2D & p, const Arc2D & a)
{
    const Vector2D v = p - a.circle().center();
    if (arc2D_periodic_include(~v, a.a(), a.b()))
    {
        const Point2D i = a.circle().center() + Versor2D(v) * a.circle().radius();
        return dist2(p, i);
    }
    else
    {
        return std::min(dist2(p, a.pointA()), dist2(p, a.pointB()));
    }
}


CXM_NS_END
