#ifndef _MYENUM_HPP_
#define _MYENUM_HPP_

#include "enum.hpp"
#include "bitmask.hpp"

#define C4_ENUM() // this tags enums for extraction

/** bla bla */
C4_ENUM()
typedef enum {
  // bla foo
  FOO = 0,
  /** bla bar */
  BAR,
  BAZ, // bla baz
} MyEnum;

/** yada yada */
C4_ENUM()
enum class MyEnumClass {
  FOO = 0,
  BAR,
  BAZ,
};

// regen:GENERATED:(BEGIN). DO NOT EDIT THE BLOCK BELOW. WILL BE OVERWRITTEN!
/** enum: auto-generated from myenum.hpp:10: C4_ENUM: MyEnum */
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
/** enum: auto-generated from myenum.hpp:20: C4_ENUM: MyEnumClass */
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
// regen:GENERATED:(END). DO NOT EDIT THE BLOCK ABOVE. WILL BE OVERWRITTEN!
#endif // !_MYENUM_HPP_

