// YAL zeldan

#include <limits>


#include "stats.hxx"

#include <cxu/mymacros.hxx>
#include "mymacros.hxx"

using namespace std;
using namespace cxu;

CXM_NS_BEGIN


Stat::Stat(Compute ops)
{
    clear();
    _ops = ops;
}

void Stat::add(const double x)
{
    if (_ops & SUM) _sum += x;
    if (_ops & SUM2) _qsum += (sqr(x));

    if (_ops & MINMAX)
    {
        if (_count)
        {
            if (x < _min)
            {
                _nMin = _count;
                _min = x;
            }

            if (x > _max)
            {
                _nMax = _count;
                _max = x;
            }
        }
        else
        {
            _min = _max = x;
            _nMin = _nMax = 0;
        }
    }
    ++_count;
}

int Stat::count() const
{
    return _count;
}

double Stat::sum() const
{
    ASSERT((_ops & SUM) != 0);
    return _sum;
}

double Stat::sum2() const
{
    ASSERT((_ops & SUM2) != 0);
    return _qsum;
}

double Stat::average() const
{
    ASSERT((_ops & SUM) != 0);
    ASSERT(_count != 0);
    return _sum / _count;
}

double Stat::average2() const
{
    ASSERT((_ops & SUM2) != 0);
    ASSERT(_count != 0);
    return _qsum / _count;
}

double Stat::min() const
{
    ASSERT((_ops & MINMAX) != 0);
    ASSERT(_count != 0);
    return _min;
}

double Stat::max() const
{
    ASSERT((_ops & MINMAX) != 0);
    ASSERT(_count != 0);
    return _max;
}

double Stat::mid() const
{
    ASSERT((_ops & MINMAX) != 0);
    ASSERT(_count != 0);
    return (_min + _max) / 2;
}

double Stat::sigma() const
{
    ASSERT((_ops & SUM) != 0);
    ASSERT((_ops & SUM2) != 0);
    ASSERT(_count != 0);
    return ::sqrt(_qsum / _count - sqr(_sum / _count));
}

double Stat::delta() const
{
    ASSERT((_ops & MINMAX) != 0);
    ASSERT(_count != 0);
    return (_max - _min);
}

void Stat::clear()
{
    _count = 0;
    _sum = 0;
    _qsum = 0;
    _min = std::numeric_limits<double>::quiet_NaN();
    _max = std::numeric_limits<double>::quiet_NaN();
    _nMin = -1;
    _nMax = -1;
}

int Stat::nMin()
{
    ASSERT((_ops & MINMAX) != 0);
    return _nMin;
}

int Stat::nMax()
{
    ASSERT((_ops & MINMAX) != 0);
    return _nMax;
}



//*/////////////////////////////////////

Stat2D::Stat2D()
{
    clear();
}

void Stat2D::add(const Vector2D & v)
{
    _sum += v;
    _qsum += (v.qdr());
    ++_count;
}

int Stat2D::count() const
{
    return _count;
}

const Vector2D Stat2D::sum() const
{
    return _sum;
}

double Stat2D::sum2() const
{
    return _qsum;
}

const Vector2D Stat2D::average() const
{
    ASSERT(_count != 0);
    return _sum / _count;
}

double Stat2D::average2() const
{
    ASSERT(_count != 0);
    return _qsum / _count;
}

double Stat2D::sigma() const
{
    ASSERT(_count != 0);
    return ::sqrt(_qsum / _count - (_sum / _count).qdr());
}

void Stat2D::clear()
{
    _count = 0;
    _sum = Vector2D::O;
    _qsum = 0;
}



//*/////////////////////////////////////

Stat3D::Stat3D()
{
    clear();
}

void Stat3D::add(const Vector3D & v)
{
    _sum += v;
    _qsum += (v.qdr());
    ++_count;
}

int Stat3D::count() const
{
    return _count;
}

const Vector3D Stat3D::sum() const
{
    return _sum;
}

double Stat3D::sum2() const
{
    return _qsum;
}

const Vector3D Stat3D::average() const
{
    ASSERT(_count != 0);
    return _sum / _count;
}

double Stat3D::average2() const
{
    ASSERT(_count != 0);
    return _qsum / _count;
}

double Stat3D::sigma() const
{
    ASSERT(_count != 0);
    return ::sqrt(_qsum / _count - (_sum / _count).qdr());
}

void Stat3D::clear()
{
    _count = 0;
    _sum = Vector3D::O;
    _qsum = 0;
}


CXM_NS_END
