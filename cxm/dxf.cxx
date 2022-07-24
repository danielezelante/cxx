// YAL zeldan

#include <cxu/cxu.hxx>

#include "dxf.hxx"

#include "linear2d.hxx"
#include "conic2d.hxx"

#include <cxu/mymacros.hxx>
#include "mymacros.hxx"

using namespace std;
using namespace cxu;

CXM_NS_BEGIN

Dxf::Dxf(TextOutput & text, double scale)
: _text(text)
, _scale(scale)
{
    fileHead();
}

Dxf::~Dxf()
{
    fileTail();
}

void Dxf::fileHead()
{
    writeln("  0");
    writeln("SECTION");
    writeln("  2");
    writeln("ENTITIES");
}

void Dxf::fileTail()
{
    writeln("  0");
    writeln("ENDSEC");
    writeln("  0");
    writeln("EOF");
}

void Dxf::polyHead(bool closed)
{
    writeln("  0");
    writeln("POLYLINE");
    writeLayer();
    writeln(" 66");
    writeln("1");
    writeln(" 10");
    writeln("0.0");
    writeln(" 20");
    writeln("0.0");
    writeln(" 30");
    writeln("0.0");
    writeln(" 70");
    writeln(closed ? "1" : "0");
}

void Dxf::polyTail()
{
    writeln("  0");
    writeln("SEQEND");
}

void Dxf::writeLayer()
{
    writeln("  8");
    writeln("0");
}

void Dxf::writeCoord(const Point2D & p, char w)
{
    char buf[4] = {' ', 'n', w, '\0'};
    buf[1] = '1';
    writeln(buf);
    writeReal(p.x());
    buf[1] = '2';
    writeln(buf);
    writeReal(p.y());
}

void Dxf::writeVertex(const Point2D & p, double angle)
{
    writeln("  0");
    writeln("VERTEX");
    writeLayer();
    writeCoord(p, '0');
    writeln(" 42");
    writeAngle(tan(angle / 4.0));
}

void Dxf::writeReal(double x)
{
    writeln(ftod(x * _scale));
}

void Dxf::writeAngle(double x)
{
    writeln(ftod(x));
}

void Dxf::writeRatio(double x)
{
    writeln(ftod(x));
}

void Dxf::draw(const Point2D & p)
{
    writeln("  0");
    writeln("POINT");
    writeLayer();
    writeCoord(p, '0');
}

void Dxf::draw(const Segment2D & k)
{
    writeln("  0");
    writeln("LINE");
    writeLayer();
    writeCoord(k.a(), '0');
    writeCoord(k.b(), '1');
}

void Dxf::draw(const Circle2D & k)
{
    writeln("  0");
    writeln("CIRCLE");
    writeLayer();
    writeCoord(k.center(), '0');
    writeln(" 40");
    writeReal(k.radius());
}

void Dxf::draw(const Arc2D & k)
{
    writeln("  0");
    writeln("ARC");
    writeLayer();
    writeCoord(k.circle().center(), '0');
    writeln(" 40");
    writeReal(k.circle().radius());
    writeln(" 50");
    writeAngle(k.a());
    writeln(" 51");
    writeAngle(k.b());
}

void Dxf::draw(const Polygon2D & k)
{
    polyHead(true);
    for (const auto & it : k.points())
        writeVertex(it, 0);
    polyTail();
}


CXM_NS_END

