#ifndef __FOOBAR_HPP__
#define __FOOBAR_HPP__

#include "enum.hpp"

enum FooBar1 {
    FOO1 = 1,
    BAR1 = 4,
    BAZ1 = FOO1|BAR1,
    BUT1,
};
template<>
inline EnumSymbols< FooBar1 > const esyms< FooBar1 >()
{
    static typename EnumSymbols< FooBar1 >::Sym const pairs[] = {
        {FOO1, "FOO1"},
        {BAR1, "BAR1"},
        {BAZ1, "BAZ1"},
        {BUT1, "BUT1"},
    };
    return EnumSymbols< FooBar1 >(pairs);
}


enum class FooBar2
{
    FOO2 = 1,
    BAR2 = 4,
    BAZ2 = FOO2|BAR2,
    BUT2,
};
template<>
inline EnumSymbols< FooBar2 > const esyms< FooBar2 >()
{
    static typename EnumSymbols< FooBar2 >::Sym const pairs[] = {
        {FooBar2::FOO2, "FooBar2::FOO2"},
        {FooBar2::BAR2, "FooBar2::BAR2"},
        {FooBar2::BAZ2, "FooBar2::BAZ2"},
        {FooBar2::BUT2, "FooBar2::BUT2"},
    };
    return EnumSymbols< FooBar2 >(pairs);
}
template<>
inline size_t eoffs< FooBar2 >()
{
    return strlen("FooBar2::");
}

enum FooBar3 {
    FOO3 = 1,
    BAR3 = 4,
    BAZ3 = FOO3|BAR3,
    BUT3,
};
#define _FOOBAR3(m) \
m(FOO3),\
m(BAR3),\
m(BAZ3),\
m(BUT3)
A5_ENUM_STRS(FooBar3, _FOOBAR3)

enum class FooBar4 {
    FOO4 = 1,
    BAR4 = 4,
    BAZ4 = FOO4|BAR4,
    BUT4,
};
#define _FOOBAR4(m) \
m(FooBar4::FOO4),\
m(FooBar4::BAR4),\
m(FooBar4::BAZ4),\
m(FooBar4::BUT4),
A5_ENUM_CLASS_STRS(FooBar4, _FOOBAR4)


#define _FOOBAR5(m) \
m(,FOO5, = 1),\
m(,BAR5, = 4),\
m(,BAZ5, = FOO5|BAR5),\
m(,BUT5,),
C4_ENUM(FooBar5, _FOOBAR5)

#define _FOOBAR6(m) \
m(FooBar6::,FOO6, = 1),\
m(FooBar6::,BAR6, = 4),\
m(FooBar6::,BUT6, = FOO6|BAR6),
C4_ENUM_CLASS(FooBar6, _FOOBAR6)

/*
C5_ENUM(FooBar7,
    FOO7,=1,
    BAR7,=4,
    BAZ7,=FOO7|BAZ7)

C5_ENUM_CLASS(FooBar8,
    FOO8,=1,
    BAR8,=4,
    BAZ8,=FOO8|BAZ8)
*/
#endif // __FOOBAR_HPP__
