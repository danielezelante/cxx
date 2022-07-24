// YAL zeldan

#include <stdlib.h>

#include <iostream>
#include <iomanip>


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>

using namespace std;
using namespace cxu;

class TestString : public Test
{
public:

    TestString()
    {
    }

    void vrun() override
    {
        quoteunquote();
        zunquote();
        zdqparse();
        begs();
        pattern();
        myquoteifneeded();
        myspacecompose();
        ipaddress();
        doxsprintf();
    }


protected:

    void quoteunquote()
    {
        TESTINFO("BEGIN");
        quq("test");
        quq("test\nline");
    }

    void zunquote()
    {
        TESTINFO("BEGIN");
        CHECKSTRING("foobar", unquote("\"foo\"   \"bar\""));
    }

    void zdqparse()
    {
        const char * s = "micio \"gatto\" zebra\"delfino\" \"sierra\\\"tango\" #comm";
        const std::vector<string> v = dqparse(s, '#');
        const size_t z = v.size();
        CHECKSIZE(5, z);

        if (z > 0) CHECKSTRING("micio", v[0]);
        if (z > 1) CHECKSTRING("\"gatto\"", v[1]);
        if (z > 2) CHECKSTRING("zebra", v[2]);
        if (z > 3) CHECKSTRING("\"delfino\"", v[3]);
        if (z > 4) CHECKSTRING("\"sierra\\\"tango\"", v[4]);
    }

    void quq(const char * s)
    {
        const std::string q = quote(s);
        const std::string t = unquote(q);
        CHECKSTRING(s, t);
    }

    void begs()
    {
        CHECKBOOL(true, beginsWith(string("mariangela"), "maria"));
        CHECKBOOL(false, beginsWith(string("mariangela"), "marta"));
        CHECKBOOL(true, beginsWith(string("silvia"), "silvia"));
        CHECKBOOL(false, beginsWith(string("paola"), "nicoletta"));
        CHECKBOOL(false, beginsWith(string("annna"), "annabella"));
    }

    void pattern()
    {
        {
            const vector<string> v = patternparse(string(), "");
            CHECKSIZE(0, v.size());
        }

        {
            const vector<string> v = patternparse(string(), "anything");
            CHECKSIZE(0, v.size());
        }

        {
            const vector<string> v = patternparse(string(), "%$");
            CHECKSIZE(0, v.size());
        }

        {
            const vector<string> v = patternparse(string("jane"), "%$");
            CHECKSIZE(1, v.size());
            if (!v.empty() > 0) CHECKSTRING("jane", v[0]);
        }

        {
            const vector<string> v = patternparse(string("emma watson"), "%$ %$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("emma", v[0]);
                CHECKSTRING("watson", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("jennifer-lopez"), "%$-%$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("jennifer", v[0]);
                CHECKSTRING("lopez", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("nina   -       moric"), "%$ - %$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("nina", v[0]);
                CHECKSTRING("moric", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("tizio caio sempronio"), "%$ %$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("tizio", v[0]);
                CHECKSTRING("caio sempronio", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("tizio caio sempronio"), "%$ %$ ");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("tizio", v[0]);
                CHECKSTRING("caio", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("hermione granger"), "%$ %$ %$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("hermione", v[0]);
                CHECKSTRING("granger", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("harry%potter"), "%$%%%$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("harry", v[0]);
                CHECKSTRING("potter", v[1]);
            }
        }

        {
            const vector<string> v = patternparse(string("draco % malfoy"), "%$ %% %$");
            CHECKSIZE(2, v.size());
            if (v.size() >= 2)
            {
                CHECKSTRING("draco", v[0]);
                CHECKSTRING("malfoy", v[1]);
            }
        }

    }

    void myquoteifneeded()
    {
        {
            const char * k = "margherita";
            const string x = k;
            const string y = quoteifneeded(x);
            CHECKSTRING(k, y);
        }

        {
            const string x("rosa bianca");
            const string y = quoteifneeded(x);
            CHECKSTRING("\"rosa bianca\"", y);
        }



    }

    void myspacecompose()
    {
        vector<string> a;
        a.push_back("pippo");
        a.push_back("pluto");

        const string s = trim(spacecompose(a.begin(), a.end()));
        CHECKSTRING("pippo pluto", s);
    }

    void ipaddress()
    {
        string host;
        string port;
        int k;

        k = split_hostport("", host, port);
        CHECKINT(AF_UNSPEC, k);

        k = split_hostport("10.74.1.99:80", host, port);
        CHECKINT(AF_INET, k);
        CHECKSTRING("10.74.1.99", host);
        CHECKSTRING("80", port);

        k = split_hostport(" 10.74.1.99:80 ", host, port);
        CHECKINT(AF_INET, k);
        CHECKSTRING("10.74.1.99", host);
        CHECKSTRING("80", port);

        k = split_hostport(" 10.74.1.99 : 80 ", host, port);
        CHECKINT(AF_INET, k);
        CHECKSTRING("10.74.1.99", host);
        CHECKSTRING("80", port);

        k = split_hostport(" 10.74.1.99  ", host, port);
        CHECKINT(AF_INET, k);
        CHECKSTRING("10.74.1.99", host);
        CHECKSTRING("", port);

        k = split_hostport(" 10.74.1.99  :", host, port);
        CHECKINT(AF_INET, k);
        CHECKSTRING("10.74.1.99", host);
        CHECKSTRING("", port);


        k = split_hostport(" : 80  ", host, port);
        CHECKINT(AF_UNSPEC, k);
        CHECKSTRING("80", port);

        k = split_hostport("[fe80::217:31ff:f38b:dc73]:80", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("80", port);

        k = split_hostport(" [fe80::217:31ff:f38b:dc73]:80 ", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("80", port);

        k = split_hostport(" [fe80::217:31ff:f38b:dc73] : 80 ", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("80", port);

        k = split_hostport(" [ fe80::217:31ff:f38b:dc73 ] : 80 ", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("80", port);

        k = split_hostport(" [ fe80::217:31ff:f38b:dc73 ] ", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("", port);

        k = split_hostport(" [ fe80::217:31ff:f38b:dc73 ] : ", host, port);
        CHECKINT(AF_INET6, k);
        CHECKSTRING("fe80::217:31ff:f38b:dc73", host);
        CHECKSTRING("", port);


    }

    void doxsprintf()
    {
        const std::string s = xsprintf("%d", 74);
        CHECKSTRING("74", s);
    }


};




static TestString s_test_string;

Test * test_string()
{
    return &s_test_string;
}

//.
