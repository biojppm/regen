#ifndef _MYENUM_HPP_
#define _MYENUM_HPP_

#include "enum.hpp"
#include "bitmask.hpp"

/** bla bla */
C4_ENUM() // tag this enum for extraction
typedef enum {
  // bla foo
  FOO = 0,
  /** bla bar */
  BAR,
  BAZ,
  BOD,
} MyEnum;

/** yada yada */
C4_ENUM() // tag this enum for extraction
enum class MyEnumClass {
  FOO = 0,
  BAR,
  BAZ,
  BOD
};

// regen:GENERATED:(BEGIN). DO NOT EDIT THE BLOCK BELOW. WILL BE OVERWRITTEN!
/** enum: auto-generated from myenum.hpp:7: C4_ENUM: MyEnum */
template<> inline const EnumPairs< MyEnum > enum_pairs()
{
    static const EnumAndName< MyEnum > vals[] = {
        { FOO, "FOO"},
        { BAR, "BAR"},
        { BAZ, "BAZ"},
        { BOD, "BOD"},
    };
    EnumPairs< MyEnum > r(vals);
    return r;
}
/** enum: auto-generated from myenum.hpp:15: C4_ENUM: MyEnumClass */
template<> inline const EnumPairs< MyEnumClass > enum_pairs()
{
    static const EnumAndName< MyEnumClass > vals[] = {
        { MyEnumClass::FOO, "MyEnumClass::FOO"},
        { MyEnumClass::BAR, "MyEnumClass::BAR"},
        { MyEnumClass::BAZ, "MyEnumClass::BAZ"},
        { MyEnumClass::BOD, "MyEnumClass::BOD"},
    };
    EnumPairs< MyEnumClass > r(vals);
    return r;
}
// regen:GENERATED:(END). DO NOT EDIT THE BLOCK ABOVE. WILL BE OVERWRITTEN!
#endif // !_MYENUM_HPP_

