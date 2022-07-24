// YAL zeldan

#ifndef CXU_DXF_HXX
#define CXU_DXF_HXX

#include <cxu/cxu.hxx>

#include "types.hxx"

CXM_NS_BEGIN

class Dxf : cxu::Entity
{
public:
    Dxf(cxu::TextOutput & text, double scale = 1);
    virtual ~Dxf();

    //TODO void draw(const Polyarc2D &);
    void draw(const Segment2D &);
    void draw(const Circle2D &);
    void draw(const Arc2D &);
    void draw(const Point2D &);
    void draw(const Polygon2D &);
    //TODO void draw(const std::vector<Point2D> &, bool closed);
    void draw(const Ellipse2D &);

    void drawApprox(const Ellipse2D &, size_t);


protected:
    cxu::TextOutput & _text;

    const double _scale;

    void fileHead();
    void fileTail();
    void polyHead(bool closed);
    void polyTail();

    void writeReal(double);
    void writeAngle(double);
    void writeRatio(double);
    void writeLayer();
    void writeCoord(const Point2D & p, char w);
    void writeVertex(const Point2D & p, double angle);

    void writeln(const char * str)
    {
        _text.writeln(str);
    }

    void writeln(const std::string & str)
    {
        _text.writeln(str);
    }
};

CXM_NS_END

#endif
