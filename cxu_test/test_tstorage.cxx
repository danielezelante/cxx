// YAL zeldan

#include <unistd.h>

#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>
using namespace cxu;

class TestTStorage : public Test
{
public:

    TestTStorage()
    {
    }

    void vrun() override
    {
        const_0000();
        int_0000();
        int_0001();
        string_0000();
        block_0000();
    }



protected:

    void const_0000()
    {
        Memory buffer;
        const char * c = "CHECK";
        {
            TOStorage s(buffer);
            s << c;
        }

        {
            TIStorage s(buffer);
            s >> c;
        }
    }

    void int_0000()
    {
        Memory buffer;
        int c = 12345678;
        {
            TOStorage s(buffer);
            s << c;
        }

        int v = 0;
        {
            TIStorage s(buffer);
            s >> v;
        }
        CHECKINT(c, v);
    }

    void int_0001()
    {
        Memory buffer;
        int n = 100;
        {
            TOStorage s(buffer);
            LOOP(j, n)
            s << j*j;
        }
        {
            TIStorage s(buffer);

            LOOP(j, n)
            {
                int k;
                s >> k;
                CHECKINT(j*j, k);
            }
        }
    }

    void string_0000()
    {
        TESTINFO("BEGIN");
        Memory buffer;
        std::vector<std::string> v;
        v.push_back(std::string("string"));
        v.push_back(std::string("abc\ndef\rghi\\jkl\'mno\"pqr"));
        const char xxx[] = {1, 16, 31, 32, 33, 126, 127, static_cast<char> (-1), static_cast<char> (-2), 0};
        v.push_back(std::string(xxx));
        {
            TOStorage s(buffer);
            for (auto const & it : v)
                s << it;
        }

        TextOutputFILE tof(stdout);
        tof.dump(buffer, "# => ");
        std::string sd(buffer.cptr(), buffer.size());
        TESTINFO(sd);

        {
            TIStorage s(buffer);
            std::string str;
            for (auto const & it : v)
            {
                s >> str;
                CHECKSTRING(it, str);
            }
        }
    }

    void block_0000()
    {
        const int c1 = 12345678;
        const int c2 = 77747774;
        const std::string filename =
                Format("%$/%$-TestTStorage_block_0000.txt")
                % _suite->tmpdir()
                % utod(getpid());
        {
            printf("# %s writing\n", filename.data());
            FileMappingBuffer buffer(filename.data(), true, 64 * 1024);
            {
                TOStorage s(buffer);
                s << TOStorage::DEC;
                s << TOStorage::BEGIN << c1 << TOStorage::END << c2;
                s << TOStorage::POP;
            }
        }

        {
            printf("# %s reading 1\n", filename.data());
            const ConstFileMappingData buffer(filename.data());
            {
                int v1 = 0;
                int v2 = 0;
                TIStorage s(buffer);
                try
                {
                    s >> TIStorage::BEGIN >> v1 >> TIStorage::END;
                }
                catch (const Exception & xe)
                {
                    printf("# exception %s\n", xe.str().data());
                    s >> TIStorage::ABORT;
                }
                s >> v2;
                CHECKINT(c1, v1);
                CHECKINT(c2, v2);
            }
        }

        {
            printf("# %s reading 2\n", filename.data());
            const ConstFileMappingData buffer(filename.data());
            {
                int v2 = 0;
                TIStorage s(buffer);
                s >> TIStorage::BEGIN >> TIStorage::ABORT >> v2;
                CHECKINT(c2, v2);
            }
        }

        {
            printf("# %s reading 3\n", filename.data());
            const ConstFileMappingData buffer(filename.data());
            {
                int v1 = 0;
                int v1bis = 0;
                int v2 = 0;
                TIStorage s(buffer);
                try
                {
                    s >> TIStorage::BEGIN >> v1 >> v1bis >> TIStorage::END;
                }
                catch (const Exception & xe)
                {
                    printf("# exception %s\n", xe.str().data());
                    s >> TIStorage::ABORT;
                }
                s >> v2;
                CHECKINT(c1, v1);
                CHECKINT(c2, v2);
            }
        }


    }



};

static TestTStorage s_test_tstorage;

Test * test_tstorage()
{
    return &s_test_tstorage;
}


//.
