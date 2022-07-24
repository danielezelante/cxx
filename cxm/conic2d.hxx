// YAL zeldan

#ifndef CXM_CONIC2D_HXX
#define CXM_CONIC2D_HXX

#include <cxu/cxu.hxx>

#include "real.hxx"
#include "linear2d.hxx"

CXM_NS_BEGIN

class Circle2D : public cxu::Serialization<Circle2D>
{
    CXU_DECLARE_SERIALIZABLE(Circle2D);

private:
    Point2D _p;
    double _r;

public:

    Circle2D() : _r(double())
    {
    }

    Circle2D(const Point2D & p, double r) : _p(p), _r(r)
    {
    }
    static const Circle2D U;

    const Point2D & center() const
    {
        return _p;
    }

    double radius() const
    {
        return _r;
    }

    bool co(const Point2D & p) const
    {
        return dist2(p, _p) < cxu::sqr(_r);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class Arc2D : public cxu::Serialization<Arc2D>
{
    CXU_DECLARE_SERIALIZABLE(Arc2D);

private:
    Circle2D _c;
    double _a, _b;


public:

    Arc2D() : _a(double()), _b(double())
    {
    }

    Arc2D(const Circle2D & c, double va, double vb) : _c(c), _a(angle_Pi_Pi(va)), _b(angle_Pi_Pi(vb))
    {
    }

    Arc2D(const Point2D & p, double r, double va, double vb) : _c(p, r), _a(angle_Pi_Pi(va)), _b(angle_Pi_Pi(vb))
    {
    }

    const Circle2D & circle() const
    {
        return _c;
    }

    double a() const
    {
        return _a;
    }

    double b() const
    {
        return _b;
    }

    double length() const
    {
        return (_b - _a) * _c.radius();
    }
    const Point2D pointA() const;
    const Point2D pointB() const;

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};

class Ellipse2D : public cxu::Serialization<Ellipse2D>
{
    CXU_DECLARE_SERIALIZABLE(Ellipse2D);

private:
    Transform2D _t;


public:

    Ellipse2D() = default;

    Ellipse2D(const Circle2D & c) : _t(Matrix22(c.radius(), 0, 0, c.radius()), Vector2D(c.center()))
    {
    }

    explicit Ellipse2D(const Transform2D & tr) : _t(tr)
    {
    }

    const Transform2D & t() const
    {
        return _t;
    }

    const Point2D center() const
    {
        return Point2D(_t.vector());
    }
    const Vector2D axis() const;
    double ratio() const;

    bool co(const Point2D & p)
    {
        return (!(Vector2D(p*~_t))) < 1;
    }

    Ellipse2D & operator*=(const Transform2D &);

    const Ellipse2D operator*(const Transform2D & tr) const
    {
        return Ellipse2D(_t * tr);
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);
};

class EllipticArc2D : public cxu::Serialization<EllipticArc2D>
{
    CXU_DECLARE_SERIALIZABLE(EllipticArc2D);

private:
    Ellipse2D _e;
    double _a, _b; // angoli dell'arco di circonferenza unitaria


public:

    EllipticArc2D() : _a(double()), _b(double())
    {
    }

    EllipticArc2D(const Arc2D & k) : _e(k.circle()), _a(k.a()), _b(k.b())
    {
    }

    double a() const
    {
        return _a;
    }

    double b() const
    {
        return _b;
    }

    const Ellipse2D & ellipse() const
    {
        return _e;
    }

    EllipticArc2D & operator*=(const Transform2D &);
    const EllipticArc2D operator*(const Transform2D &) const;

    const Point2D pointA() const
    {
        return Point2D(Vector2D::polar(1, _a)) * _e.t();
    }

    const Point2D pointB() const
    {
        return Point2D(Vector2D::polar(1, _b)) * _e.t();
    }

    void _Serialization_serialize(cxu::BOStorage &) const;
    void _Serialization_serialize(cxu::BIStorage &);
    void _Serialization_serialize(cxu::TOStorage &) const;
    void _Serialization_serialize(cxu::TIStorage &);

};


std::pair<Point2D, Point2D> inters(const Circle2D & c, const Line2D & r);
std::pair<Point2D, Point2D> inters(const Circle2D & c, const SemiLine2D & r);
std::pair<Point2D, Point2D> inters(const Circle2D & c, const Segment2D & r);
std::pair<Point2D, Point2D> inters(const Arc2D & a, const Line2D & r);
std::pair<Point2D, Point2D> inters(const Arc2D & a, const SemiLine2D & r);
std::pair<Point2D, Point2D> inters(const Arc2D & arc, const Segment2D & seg);

std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const Line2D & r);
std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const SemiLine2D & r);
std::pair<Point2D, Point2D> inters(const Ellipse2D & c, const Segment2D & r);

std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const Line2D & r);
std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const SemiLine2D & r);
std::pair<Point2D, Point2D> inters(const EllipticArc2D & c, const Segment2D & r);


std::pair<Point2D, Point2D> inters(const Circle2D &, const Circle2D &);
std::pair<Point2D, Point2D> inters(const Arc2D &, const Arc2D &);

double dist2(const Point2D & p, const Circle2D & c);

inline double dist(const Point2D & p, const Circle2D & c)
{
    return ::sqrt(dist2(p, c));
}

double dist2(const Point2D & p, const Arc2D & a);

inline double dist(const Point2D & p, const Arc2D & a)
{
    return ::sqrt(dist2(p, a));
}

CXM_NS_END

#endif
