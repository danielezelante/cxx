// YAL zeldan

#ifndef CXU_MYSTD_HXX
#define CXU_MYSTD_HXX

#include <map>
#include <list>
#include <set>
#include <vector>

#include "defs.hxx"
#include "globals.hxx"





#define CXU_LOOP(V,N) for(CXU_NS::remove_const<__typeof__(N)>::type V=0;((V)<(N));++(V))

CXU_NS_BEGIN

template <typename T> struct type_self
{
    typedef T type;
};

template<typename T> struct remove_const
{
    typedef T type;
};

template<typename T> struct remove_const<T const>
{
    typedef T type;
};

template <class T> std::set<T> set_difference(const std::set<T> & a, const std::set<T> & b)
{
    std::vector<T> va(a.size());
    std::vector<T> vb(b.size());
    std::vector<T> vc(Max(a.size(), b.size()));

    typename std::set<T>::const_iterator cits;
    typename std::vector<T>::iterator itv;

    for (cits = a.begin(), itv = va.begin(); cits != a.end() && itv != va.end(); ++cits, ++itv) *itv = *cits;
    for (cits = b.begin(), itv = vb.begin(); cits != b.end() && itv != vb.end(); ++cits, ++itv) *itv = *cits;

    itv = set_difference(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(itv - vc.begin());

    return vc;
}

template <class T> std::set<T> set_union(const std::set<T> & a, const std::set<T> & b)
{
    std::vector<T> va(a.size());
    std::vector<T> vb(b.size());
    std::vector<T> vc(a.size() + b.size());

    typename std::set<T>::const_iterator cits;
    typename std::vector<T>::iterator itv;

    for (cits = a.begin(), itv = va.begin(); cits != a.end() && itv != va.end(); ++cits, ++itv) *itv = *cits;
    for (cits = b.begin(), itv = vb.begin(); cits != b.end() && itv != vb.end(); ++cits, ++itv) *itv = *cits;

    itv = set_union(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(itv - vc.begin());

    return vc;
}

template <class T> std::set<T> set_intersection(const std::set<T> & a, const std::set<T> & b)
{
    std::vector<T> va(a.size());
    std::vector<T> vb(b.size());
    std::vector<T> vc(MIN(a.size(), b.size()));

    typename std::set<T>::const_iterator cits;
    typename std::vector<T>::iterator itv;

    for (cits = a.begin(), itv = va.begin(); cits != a.end() && itv != va.end(); ++cits, ++itv) *itv = *cits;
    for (cits = b.begin(), itv = vb.begin(); cits != b.end() && itv != vb.end(); ++cits, ++itv) *itv = *cits;

    itv = set_intersection(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(itv - vc.begin());

    return vc;
}


//*******************************************************

template<class X, class Y> class BiUnivoc
{
public:

    BiUnivoc()
    {
    }

    virtual ~BiUnivoc()
    {
    }

    bool insert(const X & x, const Y & y)
    {
        std::pair<typename std::map<Y, X>::iterator, bool> k1 = _mapXY.insert(make_pair(x, y));
        std::pair<typename std::map<Y, X>::iterator, bool> k2 = _mapYX.insert(make_pair(y, x));

        const bool retval = k1.second && k2.second;
        if (!retval)
        {
            if (k1.second) _mapXY.erase(k1.first);
            if (k2.second) _mapYX.erase(k2.first);
        };

        return retval;
    }

    bool eraseX(const X & x)
    {
        typename std::map<X, Y>::iterator i1 = _mapXY.find(x);
        if (i1 == _mapXY.end()) return false;
        typename std::map<Y, X>::iterator i2 = _mapYX.find(i1->second);
        assertDBG(i2 != _mapYX.end());
        _mapXY.erase(i1);
        _mapYX.erase(i2);
        return true;
    }

    bool eraseY(const Y & y)
    {
        typename std::map<Y, X>::iterator i1 = _mapYX.find(y);
        if (i1 == _mapYX.end()) return false;
        typename std::map<X, Y>::iterator i2 = _mapXY.find(i1->second);
        assertDBG(i2 != _mapXY.end());
        _mapYX.erase(i1);
        _mapXY.erase(i2);
        return true;
    }

    bool findX(const X & x, Y & y) const
    {
        typename std::map<X, Y>::const_iterator i = _mapXY.find(x);
        if (i == _mapXY.end()) return false;
        y = i->second;
        return true;
    }

    bool findY(const Y & y, X & x) const
    {
        typename std::map<Y, X>::const_iterator i = _mapYX.find(y);
        if (i == _mapYX.end()) return false;
        x = i->second;
        return true;
    }

    bool IsX(const X & x) const
    {
        typename std::map<X, Y>::const_iterator i = _mapXY.find(x);
        if (i == _mapXY.end()) return false;
        return true;
    }

    bool isY(const Y & y) const
    {
        typename std::map<Y, X>::const_iterator i = _mapYX.find(y);
        if (i == _mapYX.end()) return false;
        return true;
    }

    const X & firstX() const
    {
        typename std::map<X, Y>::const_iterator i = _mapXY.begin();
        assertDBG(i != _mapXY.end());
        return i->first;
    }

    const Y & firstY() const
    {
        typename std::map<Y, X>::const_iterator i = _mapYX.begin();
        assertDBG(i != _mapYX.end());
        return i->first;
    }

    const X & lastX() const
    {
        typename std::map<X, Y>::const_reverse_iterator i = _mapXY.rbegin();
        assertDBG(i != _mapXY.rend());
        return i->first;
    }

    const Y & lastY() const
    {
        typename std::map<Y, X>::const_reverse_iterator i = _mapYX.rbegin();
        assertDBG(i != _mapYX.rend());
        return i->first;
    }

    bool isEmpty() const
    {
        bool b1 = _mapXY.empty();
        bool b2 = _mapYX.empty();
        CXU_ASSERT(b1 == b2);
        return (b1 && b2);
    }


private:
    std::map<X, Y> _mapXY;
    std::map<Y, X> _mapYX;

};

template <typename T> class ConstView
{
public:

    ConstView(typename T::const_iterator i0, typename T::const_iterator i1) :
    _begin(i0), _end(i1)
    {
    }

    explicit ConstView(const T & c) :
    _begin(c.begin()), _end(c.end())
    {
    }

    typename T::const_iterator begin() const
    {
        return _begin;
    }

    typename T::const_iterator end() const
    {
        return _end;
    }

    typename T::size_type size() const
    {
        return _end - _begin;
    }

    bool empty() const
    {
        return _begin == _end;
    }

    typename T::const_reference operator[](typename T::size_type j) const
    {
        return *(_begin + j);
    }

protected:
    typename T::const_iterator _begin;
    typename T::const_iterator _end;

};


CXU_NS_END

#endif

//.
