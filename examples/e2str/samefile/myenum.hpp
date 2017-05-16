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
} MyBitmask;

C4_ENUM()
enum class MyBitmaskClass {
  BM_NONE = 0,
  BM_FOO = 1 << 0,
  BM_BAR = 1 << 1,
  BM_BAZ = 1 << 2,
};

// regen:GENERATED:(BEGIN). DO NOT EDIT THE BLOCK BELOW. WILL BE OVERWRITTEN!
/** enum: auto-generated from myenum.hpp:7: C4_ENUM: MyEnum */
template<> inline const EnumSymbols< MyEnum > esyms()
{
    static const EnumSymbols< MyEnum >::Sym vals[] = {
        { FOO, "FOO"},
        { BAR, "BAR"},
        { BAZ, "BAZ"},
    };
    EnumSymbols< MyEnum > r(vals);
    return r;
}

/** enum: auto-generated from myenum.hpp:14: C4_ENUM: MyEnumClass */
template<> inline const EnumSymbols< MyEnumClass > esyms()
{
    static const EnumSymbols< MyEnumClass >::Sym vals[] = {
        { MyEnumClass::FOO, "MyEnumClass::FOO"},
        { MyEnumClass::BAR, "MyEnumClass::BAR"},
        { MyEnumClass::BAZ, "MyEnumClass::BAZ"},
    };
    EnumSymbols< MyEnumClass > r(vals);
    return r;
}
template<> inline size_t eoffs_cls< MyEnumClass >()
{
    // same as strlen("MyEnumClass::")
    return 13;
}

/** enum: auto-generated from myenum.hpp:21: C4_ENUM: MyBitmask */
template<> inline const EnumSymbols< MyBitmask > esyms()
{
    static const EnumSymbols< MyBitmask >::Sym vals[] = {
        { BM_NONE, "BM_NONE"},
        { BM_FOO, "BM_FOO"},
        { BM_BAR, "BM_BAR"},
        { BM_BAZ, "BM_BAZ"},
    };
    EnumSymbols< MyBitmask > r(vals);
    return r;
}
template<> inline size_t eoffs_pfx< MyBitmask >()
{
    // same as strlen("BM_")
    return 3;
}

/** enum: auto-generated from myenum.hpp:29: C4_ENUM: MyBitmaskClass */
template<> inline const EnumSymbols< MyBitmaskClass > esyms()
{
    static const EnumSymbols< MyBitmaskClass >::Sym vals[] = {
        { MyBitmaskClass::BM_NONE, "MyBitmaskClass::BM_NONE"},
        { MyBitmaskClass::BM_FOO, "MyBitmaskClass::BM_FOO"},
        { MyBitmaskClass::BM_BAR, "MyBitmaskClass::BM_BAR"},
        { MyBitmaskClass::BM_BAZ, "MyBitmaskClass::BM_BAZ"},
    };
    EnumSymbols< MyBitmaskClass > r(vals);
    return r;
}
template<> inline size_t eoffs_cls< MyBitmaskClass >()
{
    // same as strlen("MyBitmaskClass::")
    return 16;
}
template<> inline size_t eoffs_pfx< MyBitmaskClass >()
{
    // same as strlen("MyBitmaskClass::BM_")
    return 19;
}



// regen:GENERATED:(END). DO NOT EDIT THE BLOCK ABOVE. WILL BE OVERWRITTEN!
#endif // !_MYENUM_HPP_
