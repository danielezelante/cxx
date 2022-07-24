// YAL zeldan

#define _USE_MATH_DEFINES
#include <cmath>

#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>

using namespace cxu;
using namespace cxm;

class TestLinear3D : public Test
{
public:

    TestLinear3D()
    {
    }

    void vrun() override
    {
        dist_Point3D_0000();
        dist_Point3D_Segment3D_0000();
        dist_Point3D_Segment3D_0001();
        dist_Point3D_Segment3D_0002();
        dist_Segment3D_0000();
        segment3D_coproj_0000();
        matrix33_inverse_0000();
        versor3D_0000();
        versor3D_0001();
        nan_0000();
        solid_0000();
        solid_0001();
        solidAngle_0000();
        solidAngle_0001();
    }



protected:

    void dist_Point3D_0000()
    {
        Point3D p1(0, 0, 0);
        Point3D p2(3, 4, 5);
        double d = dist(p1, p2);
        CHECKREAL(::sqrt(50.0), d);
    }

    void dist_Point3D_Segment3D_0000()
    {
        Segment3D s(Point3D(0, 0, 1), Point3D(2, 0, 1));
        Point3D p(1, 4, 1);
        double d = dist(p, s);
        CHECKREAL(4, d);
    }

    void dist_Point3D_Segment3D_0001()
    {
        Segment3D s(Point3D(0, 0, 1), Point3D(2, 0, 1));
        Point3D p(3, 1, 1);
        double d = dist(p, s);
        CHECKREAL(sqrt(2.0), d);
    }

    void dist_Point3D_Segment3D_0002()
    {
        Segment3D s(Point3D(0, 0, 1), Point3D(0, 0, 1));
        Point3D p(0, 1, 1);
        double d = dist(p, s);
        CHECKREAL(1.0, d);
    }

    void dist_Segment3D_0000()
    {
        Segment3D s1(Point3D(0, 0, 1), Point3D(1, 0, 1));
        Segment3D s2(Point3D(2, 0, 1), Point3D(3, 1, 1));
        double d = dist(s1, s2);
        CHECKREAL(1.0, d);
    }

    void segment3D_coproj_0000()
    {
        Segment3D s(Point3D(0, 0, 1), Point3D(2, 0, 1));
        Point3D p1(1, 5, 1);
        Point3D p2(3, 5, 1);
        Point3D p3(-1, 5, 1);
        CHECKBOOL(true, s.coproj(p1));
        CHECKBOOL(false, s.coproj(p2));
        CHECKBOOL(false, s.coproj(p3));
    }

    void matrix33_inverse_0000()
    {
        Matrix33 a(2, 3, 5, 7, 11, 13, 17, 19, 23);
        Matrix33 b = ~a;
        Matrix33 c = a*b;
        CHECKREAL(1, c.c1().x());
        CHECKREAL(0, c.c1().y());
        CHECKREAL(0, c.c1().z());
        CHECKREAL(0, c.c2().x());
        CHECKREAL(1, c.c2().y());
        CHECKREAL(0, c.c2().z());
        CHECKREAL(0, c.c3().x());
        CHECKREAL(0, c.c3().y());
        CHECKREAL(1, c.c3().z());
    }

    void versor3D_0000()
    {
        Versor3D u(3, 4, 5);
        CHECKREAL(3.0 / sqrt(50.0), u.x());
        CHECKREAL(4.0 / sqrt(50.0), u.y());
        CHECKREAL(5.0 / sqrt(50.0), u.z());
    }

    void versor3D_0001()
    {
        Versor3D u(0, 0, 0);
        CHECKBOOL(false, ::finite(u.x()));
        CHECKBOOL(false, ::finite(u.y()));
        CHECKBOOL(false, ::finite(u.z()));
    }

    void nan_0000()
    {
        Point3D p = Point3D::NaN;
        CHECKBOOL(false, p.isFinite());
        CHECKBOOL(false, ::finite(p.x()));
        CHECKBOOL(false, ::finite(p.y()));
        CHECKBOOL(false, ::finite(p.z()));
    }

    void solid_0000()
    {
        // cube
        Solid3D s;

        s.addVertex(Point3D(0, 0, 0)); // 0
        s.addVertex(Point3D(1, 0, 0)); // 1
        s.addVertex(Point3D(1, 1, 0)); // 2
        s.addVertex(Point3D(0, 1, 0)); // 3
        s.addVertex(Point3D(0, 0, 1)); // 4
        s.addVertex(Point3D(1, 0, 1)); // 5
        s.addVertex(Point3D(1, 1, 1)); // 6
        s.addVertex(Point3D(0, 1, 1)); // 7

        //sotto
        s.addFace(1, 0, 3);
        s.addFace(3, 2, 1);

        // sopra
        s.addFace(7, 4, 5);
        s.addFace(5, 6, 7);

        //davanti
        s.addFace(4, 0, 1);
        s.addFace(1, 5, 4);

        //dietro
        s.addFace(3, 7, 6);
        s.addFace(6, 2, 3);

        //sinitra
        s.addFace(7, 3, 0);
        s.addFace(0, 4, 7);

        //destra
        s.addFace(5, 1, 2);
        s.addFace(2, 6, 5);



        CHECKBOOL(true, s.verify());

        const std::vector<Triangle3D> & fcs = s.faces();
        CHECKINT(12, fcs.size());
        const Triangle3D & face0 = fcs[0];
        CHECKREAL(1, face0.a().x());
        CHECKREAL(0, face0.a().y());
        CHECKREAL(0, face0.a().z());
        CHECKREAL(0, face0.b().x());
        CHECKREAL(0, face0.b().y());
        CHECKREAL(0, face0.b().z());
        CHECKREAL(0, face0.c().x());
        CHECKREAL(1, face0.c().y());
        CHECKREAL(0, face0.c().z());

        const Triangle3D & face1 = fcs[1];
        CHECKREAL(0, face1.a().x());
        CHECKREAL(1, face1.a().y());
        CHECKREAL(0, face1.a().z());
        CHECKREAL(1, face1.b().x());
        CHECKREAL(1, face1.b().y());
        CHECKREAL(0, face1.b().z());
        CHECKREAL(1, face1.c().x());
        CHECKREAL(0, face1.c().y());
        CHECKREAL(0, face1.c().z());

        CHECKREAL(1, s.volume());
        CHECKREAL(6, s.area());
        Point3D c = s.baricenter();
        CHECKREAL(0.5, c.x());
        CHECKREAL(0.5, c.y());
        CHECKREAL(0.5, c.z());
        CHECKBOOL(true, s.co(c));
        CHECKBOOL(true, s.co(Point3D(0.2, 0.3, 0.4)));
        CHECKBOOL(false, s.co(Point3D(2, 3, 4)));
    }

    void solid_0001()
    {
        // tetra
        Solid3D s;

        Point3D p0 = Point3D(0, 0, 0); // 0
        Point3D p1 = Point3D(1, 0, 0); // 1
        Point3D p2 = Point3D(0.5, sqrt(3.0) / 2, 0); // 2
        Point3D p3 = Point3D(0.5, sqrt(3.0) / 6, sqrt(6.0) / 3); // 3

        s.addVertex(p0);
        s.addVertex(p1);
        s.addVertex(p2);
        s.addVertex(p3);

        //sotto
        s.addFace(0, 2, 1);
        s.addFace(0, 3, 2);
        s.addFace(0, 1, 3);
        s.addFace(1, 2, 3);

        CHECKBOOL(true, s.verify());

        CHECKREAL(sqrt(2.0) / 12, s.volume());
        CHECKREAL(sqrt(3.0), s.area());
        Point3D c = s.baricenter();
        CHECKREAL(0.5, c.x());
        CHECKREAL(sqrt(3.0) / 6, c.y());
        CHECKREAL(sqrt(6.0) / 12, c.z());
        CHECKBOOL(true, s.co(c));
        CHECKBOOL(true, s.co(Point3D(0.1, 0.1, 0.1)));
        CHECKBOOL(false, s.co(Point3D(2, 3, 4)));

        Versor3D w0(c - p0);
        Versor3D w1(c - p1);
        Versor3D w2(c - p2);

        CHECKREAL((4 * M_PI) / 4, solidAngle(w0, w1, w2));
    }

    void solidAngle_0000()
    {
        CHECKREAL((4.0 * M_PI) / 8, solidAngle(Versor3D::X, Versor3D::Y, Versor3D::Z));
    }

    void solidAngle_0001()
    {
        CHECKREAL((4 * M_PI) / 12, solidAngle(Versor3D(-1, -1, +1), Versor3D(-1, +1, +1), Versor3D(+1, -1, +1)));
    }


};

static TestLinear3D s_test;

Test * test_linear3d()
{
    return &s_test;
}


//.
