#include <cstdio>
#include <cstdlib>

#include "reflect.hpp"
#include <stdint.h>

C4_ENUM()
typedef enum {
    TE_0 = 0,
    TE_1,
    TE_2
} TestEnum_e;

C4_ENUM()
enum class TestEnumClass_e {
    TEC_0 = 0,
    TEC_1,
    TEC_2,
    TEC_3,
    TEC_4,
    TEC_5,
    TEC_6,
    TEC_7,
    TEC_8,
};

struct TestStruct
{
    C4_CLASS()
    float x, y, z;
};
template< class T >
struct TestTpl
{
    C4_CLASS()
    T r,g,b,a;
};
template< class T, class U >
struct TestTpl2
{
    C4_CLASS()
    T x;
    U y;
};
template< class T, class U, class V >
struct TestTpl3
{
    C4_CLASS()
    T x;
    U y;
    V z;
};
template< class T, class U, class V, int N >
struct TestTpl4
{
    C4_CLASS()
    T x[N];
    U y[N];
    V z[N];
};
template< class T, class U, class V, int N, template< class > class AAA >
struct TestTpl51
{
    C4_CLASS()
    T x[N];
    U y[N];
    V z[N];
    AAA< T > w;
};
template< class T, class U, class V, int N, template< class, class > class AAA >
struct TestTpl52
{
    C4_CLASS()
    T x[N];
    U y[N];
    V z[N];
    AAA< T, U > w;
};
template< class T, class U, class V, int N, template< class, class, class > class AAA >
struct TestTpl53
{
    C4_CLASS()
    T x[N];
    U y[N];
    V z[N];
    AAA< T, U, V > w;
};
template< class T, class U, class V, int N, template< class, class, class, int > class AAA >
struct TestTpl54
{
    C4_CLASS()
    T x[N];
    U y[N];
    V z[N];
    AAA< T, U, V, N > w;
};

class ThisIsATest
{
    C4_CLASS()
public:

    C4_ENUM()
    typedef enum {
        CE_0 = 0,
        CE_1,
        CE_2
    } TTestEnum_e;

private:

    bool bdata;
    char mode_data;

    C4_PROPERTY(default=0)
    int prop;

    C4_PROPERTY(longname="proplong", help="does, this and that", serialize, inspect)
    int prop2;

    float more_data;
    double yet_more_data;
    static TestEnum_e ste;

    TestStruct ts;
    TestTpl< uint32_t > ttpl;
};

template< typename T > void pqp() {}

int main(int argc, char* argv[])
{
    printf("hello");
    return 0;
}

