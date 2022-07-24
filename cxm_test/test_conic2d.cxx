// YAL zeldan

#define _USE_MATH_DEFINES
#include <cmath>

#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>



using namespace cxu;
using namespace cxm;

class TestConic2D : public Test
{
public:

    TestConic2D()
    {
    }

    void vrun() override
    {
        inters_circle_0000();
        inters_circle_0001();
        inters_circle_0002();
        inters_arc_0000();
        inters_arc_0001();
        ellipse_0000();
        ellipticarc_0000();
        ellipse_0001();
        ellipse_0002();
        ellipse_0003();
        ellipse_0004();
        ellipse_0005();
        ellipse_0006();
        ellipse_0007();
        ellipse_0008();
        ellipse_0009();
        ellipse_0010();
    }



protected:

    void inters_circle_0000()
    {
        Circle2D c1(Point2D(-1, 0), 2);
        Circle2D c2(Point2D(+1, 0), 2);
        std::pair<Point2D, Point2D> p = inters(c1, c2);
        const Point2D p1 = p.first;
        const Point2D p2 = p.second;
        CHECKREAL(0, p1.x());
        CHECKREAL(0, p2.x());
        CHECKREAL(-sqrt(3.0), std::min(p1.y(), p2.y()));
        CHECKREAL(+sqrt(3.0), std::max(p1.y(), p2.y()));
    }

    void inters_circle_0001()
    {
        const Circle2D c1(Point2D(-1 + 5, 0 + 7), 2);
        const Circle2D c2(Point2D(+1 + 5, 0 + 7), 2);
        std::pair<Point2D, Point2D> p = inters(c1, c2);
        const Point2D p1 = p.first;
        const Point2D p2 = p.second;
        CHECKREAL(0 + 5, p1.x());
        CHECKREAL(0 + 5, p2.x());
        CHECKREAL(-sqrt(3.0) + 7, std::min(p1.y(), p2.y()));
        CHECKREAL(+sqrt(3.0) + 7, std::max(p1.y(), p2.y()));
    }

    void inters_circle_0002()
    {
        Circle2D c1(Point2D(-10, 0), 2);
        Circle2D c2(Point2D(+10, 0), 2);
        std::pair<Point2D, Point2D> p = inters(c1, c2);
        const Point2D p1 = p.first;
        const Point2D p2 = p.second;
        CHECKBOOL(false, ::finite(p1.x()));
        CHECKBOOL(false, ::finite(p2.x()));
        CHECKBOOL(false, ::finite(p1.y()));
        CHECKBOOL(false, ::finite(p2.y()));
    }

    void inters_arc_0000()
    {
        Arc2D c1(Point2D(-1, 0), 2, 0, M_PI);
        Arc2D c2(Point2D(+1, 0), 2, 0, M_PI);
        std::pair<Point2D, Point2D> p = inters(c1, c2);
        const Point2D p1 = p.first;
        const Point2D p2 = p.second;
        CHECKREAL(0, p1.x());
        CHECKBOOL(false, ::finite(p2.x()));
        CHECKREAL(sqrt(3.0), p1.y());
        CHECKBOOL(false, ::finite(p2.y()));
    }

    void inters_arc_0001()
    {
        Arc2D c1(Point2D(-1, 0), 2, -M_PI / 2, M_PI / 2);
        Arc2D c2(Point2D(+1, 0), 2, M_PI / 2, -M_PI / 2);
        std::pair<Point2D, Point2D> p = inters(c1, c2);
        const Point2D p1 = p.first;
        const Point2D p2 = p.second;
        CHECKREAL(0, p1.x());
        CHECKREAL(0, p2.x());
        CHECKREAL(sqrt(3.0), std::max(p1.y(), p2.y()));
        CHECKREAL(-sqrt(3.0), std::min(p1.y(), p2.y()));
    }

    void ellipse_0000()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(2, 1)) * Transform2D::rotation(M_PI / 4));
        Segment2D s(Point2D(-10, -10), Point2D(10, 10));
        std::pair<Point2D, Point2D> p = inters(e, s);
        const Point2D & p1 = p.first;
        const Point2D & p2 = p.second;

        CHECKREAL(sqrt(2.0), std::max(p1.x(), p2.x()));
        CHECKREAL(sqrt(2.0), std::max(p1.y(), p2.x()));
        CHECKREAL(-sqrt(2.0), std::min(p1.x(), p2.x()));
        CHECKREAL(-sqrt(2.0), std::min(p1.y(), p2.x()));

    }

    void ellipticarc_0000()
    {
        EllipticArc2D e(Arc2D(Circle2D::U, -M_PI / 3, M_PI / 3));
        e *= Transform2D::scale(Vector2D(2, 1)) * Transform2D::rotation(M_PI / 4);
        Segment2D s(Point2D(-10, -10), Point2D(10, 10));
        std::pair<Point2D, Point2D> p = inters(e, s);

        CHECKREAL(sqrt(2.0), p.first.x());
        CHECKREAL(sqrt(2.0), p.first.y());
        CHECKBOOL(false, p.second.isFinite());

    }

    void ellipse_0001()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3, axis.x());
        CHECKREAL(0, axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0002()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(M_PI / 4));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 / sqrt(2.0), axis.x());
        CHECKREAL(3.0 / sqrt(2.0), axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0003()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(2, 3)));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(0, axis.x());
        CHECKREAL(3, fabs(axis.y()));
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0004()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(11, 7)));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(11, axis.x());
        CHECKREAL(0, axis.y());
        CHECKREAL(7.0 / 11.0, ratio);
    }

    void ellipse_0005()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(-M_PI / 4));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 / sqrt(2.0), axis.x());
        CHECKREAL(-3.0 / sqrt(2.0), axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0006()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(M_PI / 6));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 * (sqrt(3.0) / 2.0), axis.x());
        CHECKREAL(3.0 * (1.0 / 2.0), axis.y());

        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0007()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(-M_PI / 6));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 * (sqrt(3.0) / 2.0), axis.x());
        CHECKREAL(-3.0 * (1.0 / 2.0), axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0008()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(M_PI / 3));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 * (1.0 / 2.0), axis.x());
        CHECKREAL(3.0 * (sqrt(3.0) / 2.0), axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0009()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(-M_PI / 3));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(3.0 * (1.0 / 2.0), axis.x());
        CHECKREAL(-3.0 * (sqrt(3.0) / 2.0), axis.y());
        CHECKREAL(2.0 / 3.0, ratio);
    }

    void ellipse_0010()
    {
        Ellipse2D e(Transform2D::scale(Vector2D(3, 2)) * Transform2D::rotation(M_PI / 2));
        Vector2D axis = e.axis();
        double ratio = e.ratio();
        CHECKREAL(0.0, axis.x());
        CHECKREAL(3.0, fabs(axis.y()));
        CHECKREAL(2.0 / 3.0, ratio);
    }



};

static TestConic2D s_test;

Test * test_conic2d()
{
    return &s_test;
}


//.
