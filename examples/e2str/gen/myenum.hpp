#ifndef _MYENUM_HPP_
#define _MYENUM_HPP_

#include "enum.hpp"
#define C4_ENUM() // this tags enums for extraction

C4_ENUM()
typedef enum {
    FOO = 0,
    BAR,
    BAZ,
} MyEnum;

C4_ENUM()
enum class MyEnumClass {
    FOO = 0,
    BAR,
    BAZ,
};

C4_ENUM()
typedef enum {
    BM_NONE = 0,
    BM_FOO = 1 << 0,
    BM_BAR = 1 << 1,
    BM_BAZ = 1 << 2,
    BM_FOO_BAR = BM_FOO|BM_BAR,
    BM_FOO_BAR_BAZ = BM_FOO|BM_BAR|BM_BAZ,
} MyBitmask;

C4_ENUM()
enum class MyBitmaskClass {
    BM_NONE = 0,
    BM_FOO = 1 << 0,
    BM_BAR = 1 << 1,
    BM_BAZ = 1 << 2,
    BM_FOO_BAR = BM_FOO|BM_BAR,
    BM_FOO_BAR_BAZ = BM_FOO|BM_BAR|BM_BAZ,
};

#endif // !_MYENUM_HPP_
