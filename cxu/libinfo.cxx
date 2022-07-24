// YAL zeldan

#include <stdio.h>

#include "libinfo.hxx"

#include "string.hxx"
#include "mystd.hxx"
#include "mymacros.hxx"

#undef min
#undef max

using namespace std;

CXU_NS_BEGIN


// moved to cxu.cxx for initialization order
// std::vector<LibInfo> LibInfo::_list;


std::string LibInfo::str() const
{
    std::string s("%$ %$-%$-%$ (%$)%$ %$");
    return Format(s)
            % _product
            % _fixver
            % _repver
            % _hashver
            % _timestamp
            % (_debug ? "DBG" : "")
            % _author;
}

void LibInfo::regthis() const
{
    _list.push_back(*this);
}

void LibInfo::print(FILE * f) const
{
    fprintf(f, "%s\n", str().data());
}

const std::vector<LibInfo> & LibInfo::queryList()
{
    return _list;
}

void LibInfo::printList(FILE * f)
{
    const std::vector<LibInfo> & l = queryList();
    vector<size_t> fw;
    vector<vector<string> > elements;

    for (auto const & it : l)
    {
        std::string s = it.str();
        vector<string> sa = dqparse(s, 0);
        elements.push_back(sa);
        if (sa.size() > fw.size())
        {
            const size_t oz = fw.size();
            fw.resize(sa.size());
            for (size_t j = oz; j < fw.size(); ++j)
                fw[j] = 0;
        }

        LOOP(j, sa.size())
        fw[j] = std::max(fw[j], sa[j].size());
    }

    for (auto const & it : elements)
    {
        const vector<string> & sa = it;
        std::string q;
        LOOP(j, sa.size())
        q += ljus(sa[j], fw[j] + 1);
        fprintf(f, "%s \n", q.c_str());
    }

}

LibInfoRegister::LibInfoRegister(const LibInfo & a)
{
    //fprintf(stderr,"LibInfoRegister::LibInfoRegister(%p) _product=%s \n", &a, a._product.data());
    a.regthis();
}




CXU_NS_END

//.
