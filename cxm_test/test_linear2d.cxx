// YAL zeldan

#define _USE_MATH_DEFINES
#include <cmath>

#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>


using namespace cxu;
using namespace cxm;

class TestLinear2D : public Test
{
public:

    TestLinear2D()
    {
    }

    void vrun() override
    {
        dist_Point2D_0000();
        dist_Point2D_Segment2D_0000();
        dist_Point2D_Segment2D_0001();
        dist_Segment2D_0000();
        segment2D_coproj_0000();
        matrix22_inverse_0000();
        versor2D_0000();
        versor2D_0001();
        nan_0000();
        transform_0000();
        transform_0001();
        transform_0002();
        transform_0003();

    }


protected:

    void dist_Point2D_0000()
    {
        Point2D p1(0, 0);
        Point2D p2(3, 4);
        double d = dist(p1, p2);
        CHECKREAL(5.0, d);
    }

    void dist_Point2D_Segment2D_0000()
    {
        Segment2D s(Point2D(0, 0), Point2D(2, 0));
        Point2D p(1, 4);
        double d = dist(p, s);
        CHECKREAL(4, d);
    }

    void dist_Point2D_Segment2D_0001()
    {
        Segment2D s(Point2D(0, 0), Point2D(2, 0));
        Point2D p(3, 1);
        double d = dist(p, s);
        CHECKREAL(sqrt(2.0), d);
    }

    void dist_Segment2D_0000()
    {
        Segment2D s1(Point2D(0, 0), Point2D(1, 0));
        Segment2D s2(Point2D(2, 0), Point2D(3, 1));
        double d = dist(s1, s2);
        CHECKREAL(1.0, d);
    }

    void segment2D_coproj_0000()
    {
        Segment2D s(Point2D(0, 0), Point2D(2, 0));
        Point2D p1(1, 5);
        Point2D p2(3, 5);
        Point2D p3(-1, 5);
        CHECKBOOL(true, s.coproj(p1));
        CHECKBOOL(false, s.coproj(p2));
        CHECKBOOL(false, s.coproj(p3));
    }

    void matrix22_inverse_0000()
    {
        Matrix22 a(2, 3, 5, 7);
        Matrix22 b = ~a;
        Matrix22 c = a*b;
        CHECKREAL(1, c.c1().x());
        CHECKREAL(0, c.c1().y());
        CHECKREAL(0, c.c2().x());
        CHECKREAL(1, c.c2().y());
    }

    void versor2D_0000()
    {
        Versor2D u(3, 4);
        CHECKREAL(0.6, u.x());
        CHECKREAL(0.8, u.y());
    }

    void versor2D_0001()
    {
        Versor2D u(0, 0);
        CHECKBOOL(false, ::finite(u.x()));
        CHECKBOOL(false, ::finite(u.y()));
    }

    void nan_0000()
    {
        Point2D p = Point2D::NaN;
        CHECKBOOL(false, p.isFinite());
        CHECKBOOL(false, ::finite(p.x()));
        CHECKBOOL(false, ::finite(p.y()));
    }

    void transform_0000()
    {
        Transform2D t = Transform2D::scale(Vector2D(2, 3));

        Point2D p(5, 7);
        Point2D q = p*t;

        CHECKREAL(10, q.x());
        CHECKREAL(21, q.y());
    }

    void transform_0001()
    {
        Transform2D t(Transform2D::rotation(M_PI / 6));

        Point2D p(1, 0);
        Point2D q = p*t;

        CHECKREAL(sqrt(3.0) / 2, q.x());
        CHECKREAL(0.5, q.y());
    }

    void transform_0002()
    {
        Transform2D t(Transform2D::rotation(M_PI / 12));

        Point2D p(sqrt(2.0) / 2, sqrt(2.0) / 2);
        Point2D q = p*t;

        CHECKREAL(0.5, q.x());
        CHECKREAL(sqrt(3.0) / 2, q.y());
    }

    void transform_0003()
    {
        Transform2D t(Transform2D::rotation(M_PI / 4) * Transform2D::scale(Vector2D(2, 3)));

        Point2D p(1, 0);
        Point2D q = p*t;

        CHECKREAL(sqrt(2.0), q.x());
        CHECKREAL(sqrt(2.0)*3 / 2, q.y());
    }


};

static TestLinear2D s_test;

Test * test_linear2d()
{
    return &s_test;
}



//.
