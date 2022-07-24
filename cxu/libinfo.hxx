// YAL zeldan


#ifndef CXU_LIBINFO_HXX
#define CXU_LIBINFO_HXX

#include <vector>
#include <string>

#include <stdio.h>

#include "types.hxx"
#include "object.hxx"

CXU_NS_BEGIN
class LibInfo : public Stringable
{
public:

    LibInfo(
            const std::string & product,
            const std::string & fixver,
            const std::string & repver,
            const std::string & hashver,
            const std::string & timestamp,
            const std::string & author,
            bool debug
            ) :
    _product(product),
    _fixver(fixver),
    _repver(repver),
    _hashver(hashver),
    _timestamp(timestamp),
    _author(author),
    _debug(debug)
    {
    }

    void regthis() const;

    std::string str() const override;
    void print(FILE * f = stderr) const;

    static const std::vector<LibInfo> & queryList();
    static void printList(FILE * f = stderr);


    std::string _product;

protected:
    std::string _fixver;
    std::string _repver;
    std::string _hashver;
    std::string _timestamp;
    std::string _author;
    bool _debug;

private:
    static std::vector<LibInfo> _list;

};

class LibInfoRegister : Entity
{
public:
    explicit LibInfoRegister(const LibInfo &);
};




CXU_NS_END



// use this macro in main header file of your library (product.hxx)
#define CXU_DECLARE_LIBINFO(PRODUCT)  \
namespace PRODUCT       \
{           \
        const CXU_NS::LibInfo & libinfo(); \
}           \
        
// use this macro once in main source file of your library (product.cxx)
#define CXU_IMPLEMENT_LIBINFO(AUTHOR)        \
namespace CXU_CFG_PRODUCT           \
{                 \
        static CXU_NS::LibInfo s_libinfo        \
        (                \
                CXU_CFG_PRODUCTSTR,           \
                CXU_CFG_FIXVER, CXU_CFG_REPVER, CXU_CFG_HASHVER,   \
                CXU_CFG_TIMEIDSTR,           \
                AUTHOR,              \
                CXU_CFG_ISDEBUG            \
        );                \
        const CXU_NS::LibInfo & libinfo() {return s_libinfo;}   \
}                 \


#endif

//.
