// YAL zeldan

#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <utility>


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>

using namespace std;
using namespace cxu;

class TestDemo : public Test
{
public:

    TestDemo()
    {
    }

    void vrun() override
    {
        bstream();
        tstream();
        exception0();
        exception();
        ptr();
        base64string();
        base64mem();
        hostname();
        memory_resize();
        memory_move();
        logger();
        copy_count();
        jtime();
        version();
    }



protected:

    void bstream()
    {
        const std::string filename = Format("/tmp/cxu_test/%$-data.bin") % utod(getpid());
        FileMappingBuffer fmds(filename.c_str(), true, 1024 * 1024);
        BOStorage bos(fmds);

        bos << std::string("test");
        bos << BOStorage::BEGIN << 0x77 << BOStorage::END;

    }

    void tstream()
    {
        const std::string filename = Format("/tmp/cxu_test/%$-data.txt") % utod(getpid());
        FileMappingBuffer fmds(filename.c_str(), true, 1024 * 1024);
        TOStorage tos(fmds);

        tos << TOStorage::DEC;
        tos << std::string("test") << TOStorage::NL;
        tos << "fixed";
        tos << TOStorage::BEGIN << 77 << TOStorage::END;
        tos << TOStorage::POP;

    }

    void exception0()
    {
        try
        {
            throw 1;
        }
        catch (int x)
        {
            printf("# int exception : %d (should be 1) \n", x);
        }
    }

    void exception()
    {
        puts("# exception stack trace BEGIN");
        try
        {
            CXU_THROWTEXTEXCEPTION("test text");
        }
        catch (const Exception & ex)
        {
            ex.printStackTrace(stdout, "## ");
        }
        puts("# exception stack trace END");
    }

    void ptr()
    {
        void * p = &p;
        std::cout << std::string(Format("# pointer = %$") % ptrtoa(p)) << std::endl;
        std::cout << std::string(Format("# pointer short = %$") % ptrtoas(p)) << std::endl;
        char * q = 0;
        q += 0x12345;
        std::cout << std::string(Format("# pointer = %$") % ptrtoa(q)) << std::endl;
        std::cout << std::string(Format("# pointer short = %$") % ptrtoas(q)) << std::endl;

    }

    void base64string()
    {
        string pippo = "foobar zoo \n this is a TEST 64";
        string pluto = Base64::encode(pippo);
        string zuppa = Base64::decode(pluto);
        CXU_CHECKSTRING(pippo, zuppa);
    }

    void base64mem()
    {
        const size_t z = 16384;
        Memory a(z);
        char * p = a.ptr();
        srand(1974);
        LOOP(j, z) p[j] = static_cast<char> (rand());

        std::string b = Base64::encode(ConstSegment(a));
        Memory c;
        Base64::decode(b, c);
        CHECKINT(z, c.size());
        ConstSegment s0(a);
        ConstSegment s1(c);

        LOOP(j, z)
        if (s0[j] != s1[j]) // to reduce fuffa output
            CHECKINT(s0[j], s1[j]);
    }

    void hostname()
    {
        std::string x = getHostname();
        std::cout << std::string(Format("# hostname = %$") % x) << std::endl;
        //const InetAddress myself(x.c_str());
        //std::cout << std::string(Format("# myaddress = %$") %myself) << std::endl;
        GetAddrInfo gai("localhost", "");
        gai.hintFlags(AI_CANONNAME);
        gai.doit();
        TRACE("T1");
        unsigned j = 0;
        while (gai.next())
        {
            TRACE("T2");
            std::cout << std::string(Format("# gai[%$].address = %$") % j % gai.getAddress()) << std::endl;
            std::cout << std::string(Format("# gai[%$].family = %$") % j % gai.getFamily()) << std::endl;
            std::cout << std::string(Format("# gai[%$].socktype = %$") % j % gai.getSocktype()) << std::endl;
            std::cout << std::string(Format("# gai[%$].protocol = %$") % j % gai.getProtocol()) << std::endl;
            if (!j) std::cout << std::string(Format("# gai[%$].canonname = %$") % j % quote(gai.getCanonname())) << std::endl;
            TRACE("T2.5");
            if (gai.getFamily() == AF_INET6)
            {
                TRACE("T3");
                const Inet6SockAddress sa = gai.getAddress();
                const sockaddr_in6 * sa6 = sa.sax();

                TRACE("T4");
                std::cout << std::string(Format("# sin6_flowinfo = %$") % sa6->sin6_flowinfo) << std::endl;
                std::cout << std::string(Format("# sin6_scope_id = %$") % sa6->sin6_scope_id) << std::endl;

            }
            ++j;
        }
    }

    void memory_resize()
    {
        printf("# memory_resize \n");
        Memory a;
        Memory b;

        const size_t z = 1024 * 1024;

        size_t oj = 0;
        const clock_t t0 = clock();
        for (size_t j = 16; j < z; j += j / 3)
        {
            fprintf(stderr, "# memory_resize %zu / %zu \n", j, z);
            a.resize(j);
            b.resize(j);
            Segment sa(a);
            Segment sb(b);


            for (size_t x = oj; x < j; ++x)
            {
                sa[x] = (x * x + x);
                sb[x] = (x * x * x + x);
            }

            oj = j;

            LOOP(k, j)
            {
                if (static_cast<uint8_t> (k * k + k) != static_cast<uint8_t> (sa[k]))
                {
                    printf("@? j=%zu, k=%zu, k*k+k=%u, a[k]=%u\n", j, k, static_cast<uint8_t> (k * k + k), static_cast<uint8_t> (sa[k]));
                    return;
                }

                if (static_cast<uint8_t> (k * k * k + k) != static_cast<uint8_t> (sb[k]))
                {
                    printf("@? j=%zu, k=%zu, k*k*k+k=%u, b[k]=%u\n", j, k, static_cast<uint8_t> (k * k * k + k), static_cast<uint8_t> (sb[k]));
                    return;
                }
            }
        }
        const clock_t t1 = clock();

        const char * format = "delta_clock = %d\n";
        if (sizeof (t1) == sizeof (long))
            format = "delta_clock = %ld\n";

        printf(format, t1 - t0);
    }

    void memory_move()
    {
        printf("# memory_move \n");
        const size_t z = 17;
        Memory a(z);
        CHECKINT(z, a.size());
        Memory b = std::move(a);
        CHECKINT(z, b.size());
        CHECKINT(0, a.size());
    }

    void logger()
    {
        LOGGER.info("cxu test");
    }

    void copy_count()
    {
        LOGGER.info(Format("Buffer::get_copycount() => %$") % totalCopyCount());
    }

    void jtime()
    {
        LOGGER.info(Format("now jtime = %$") % Time::now().as_jtime());
    }

    void version()
    {
        LOGGER.info(Format("cxu version = %$") % CXU_NS::libinfo());
    }

};


static TestDemo s_test_demo;

Test * test_demo()
{
    return &s_test_demo;
}

//.
