/*
Copyright (C)  2015  Daniele Zelante  <zeldan@zeldan.net>

This file is part of cxu.

cxu is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

cxu is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxu.  If not, see <http://www.gnu.org/licenses/>

 */
/*@LICENSE*/

#include <unistd.h>

#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>
using namespace cxu;

class TestBStorage : public Test
{
public:

    TestBStorage()
    {
    }

    virtual ~TestBStorage()
    {
    }

    void vrun() override
    {
        string_0000();
        int_0000();
        block_0000();
    }


protected:

    void string_0000()
    {
        CXU_TESTINFO("begin");
        TextOutputFILE tof(stderr);

        Memory buffer;
        std::string c = "string check";
        {
            BOStorage s(buffer);
            s << c;
        }
        tof.dump(buffer, "# b: ");

        std::string v;
        ConstStringData csd(v);

        tof.dump(csd, "# v: ");
        {
            BIStorage s(buffer);
            s >> v;
        }

        CHECKSTRING(c, v);
    }

    void int_0000()
    {
        CXU_TESTINFO("begin");
        Memory buffer;
        int c = 0x12345678;
        {
            BOStorage s(buffer);
            s << c;
        }
        int v = 0;
        {
            BIStorage s(buffer);
            s >> v;
        }
        CHECKINT(c, v);
    }

    void block_0000()
    {
        CXU_TESTINFO("begin");
        const int c1 = 0x12345678;
        const int c2 = 0x77747774;
        const std::string filename =
                Format("%$/%$-TestBStorage_block_0000.bin")
                % _suite->tmpdir()
                % getpid();

        {
            printf("# %s writing\n", filename.data());
            FileMappingBuffer buffer(filename.data(), true, 64 * 1024);
            {
                BOStorage s(buffer);
                s << BOStorage::BEGIN << c1 << BOStorage::END << c2;
            }
        }

        printf("# after write, before read");

        {
            printf("# %s reading 1\n", filename.data());
            const ConstFileMappingData buffer(filename.data());
            {
                int v1 = 0;
                int v2 = 0;
                BIStorage s(buffer);
                try
                {
                    s >> BIStorage::BEGIN >> v1 >> BIStorage::END;
                }
                catch (const Exception & xe)
                {
                    printf("# exception %s\n", xe.str().data());
                    s >> BIStorage::ABORT;
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
                BIStorage s(buffer);
                s >> BIStorage::BEGIN >> BIStorage::ABORT >> v2;
                CHECKINT(c2, v2);
            }
        }

        {
            printf("# %s reading 3\n", filename.data());
            const ConstFileMappingData buffer(filename.data());
            {
                int v1 = 0;
                int v2 = 0;
                BIStorage s(buffer);
                try
                {
                    int v1bis = 0;
                    s >> BIStorage::BEGIN >> v1 >> v1bis >> BIStorage::END;
                }
                catch (const Exception & xe)
                {
                    printf("# exception %s\n", xe.str().data());
                    s >> BIStorage::ABORT;
                }
                s >> v2;
                CHECKINT(c1, v1);
                CHECKINT(c2, v2);
            }
        }

    }

};

static TestBStorage s_test_bstorage;

Test * test_bstorage()
{
    return &s_test_bstorage;
}

//.
