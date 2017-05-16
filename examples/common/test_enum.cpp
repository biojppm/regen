#include <iostream>
#include <vector>

#include "myenum.hpp"
#include "bitmask.hpp"

// a googletest stub
#define _EXPECT_EQ(expr1, expr2, cmp)                            \
    {                                                            \
        bool ok = (cmp);                                         \
        error_status |= !ok;                                     \
        if(!ok)                                                  \
        {                                                        \
            std::cerr << __FILE__ << ":" << __LINE__ << ": "     \
                      << "expected " #expr1 "=" << expr1         \
                      << " to be equal to " #expr2 "=" << expr2  \
                      << "\n";                                   \
        }                                                        \
    }
#define EXPECT_EQ(expr1, expr2)                 \
    _EXPECT_EQ((expr1), (expr2), (expr1 == expr2))
#define EXPECT_STR_EQ(expr1, expr2)                         \
    _EXPECT_EQ((expr1), (expr2), (strcmp(expr1, expr2) == 0))
#define EXPECT_BM_EQ(expr1, expr2)                         \
    _EXPECT_EQ((size_t)(expr1), (size_t)(expr2), ((size_t)expr1 == (size_t)expr2))

static int error_status = 0;

template< typename E >
void test_e2str()
{
    using I = typename std::underlying_type< E >::type;
    for(auto &p : esyms< E >())
    {
        // test a round trip
        EXPECT_EQ((I)str2e< E >(e2str(p.value)), (I)p.value);
        // test the other way around
        EXPECT_STR_EQ(e2str(str2e< E >(p.name)), p.name);
    }
}

template< typename E >
void test_bm2str()
{
    using I = typename std::underlying_type< E >::type;
    int combination_depth = 4;
    auto syms = esyms< E >();

    std::vector< int > indices;
    std::string str;
    std::vector< char > ws;
    I val = 0, res;
    size_t len;

    for(int k = 1; k <= combination_depth; ++k)
    {
        indices.clear();
        indices.resize(k);
        while(1)
        {
            str.clear();
            val = 0;
            for(auto i : indices)
            {
                if(!str.empty()) str += '|';
                str += syms[i].name;
                val |= static_cast< I >(syms[i].value);
                //printf("%d", i);
            }
            //len = bm2str< E >(val); // needed length
            //ws.resize(len);
            //bm2str< E >(val, &ws[0], len);
            //printf(": %s (%zu) %s\n", str.c_str(), (uint64_t)val, ws.data());

            res = str2bm< E >(str.data());
            EXPECT_EQ(res, val);

            len = bm2str< E >(res); // needed length
            ws.resize(len);
            bm2str< E >(val, &ws[0], len);
            res = str2bm< E >(ws.data());
            EXPECT_EQ(res, val);

            // write a string with the bitmask as an int
            int ret = snprintf(&ws[0], ws.size(), "%" PRId64, (int64_t)val);
            C4_CHECK((size_t)ret < ws.size());
            res = str2bm< E >(str.data());
            EXPECT_EQ(res, val);

            bool carry = true;
            for(int i = k-1; i >= 0; --i)
            {
                if(indices[i] + 1 < syms.size())
                {
                    ++indices[i];
                    carry = false;
                    break;
                }
                else
                {
                    indices[i] = 0;
                }
            }
            if(carry)
            {
                break;
            }
        } // while(1)
    } // for k
}
