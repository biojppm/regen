// GENERATED AUTOMATICALLY. DO NOT EDIT THIS FILE: IT WILL BE OVERWRITTEN.

#include "myenum.gen.hpp"

/** enum: auto-generated from myenum.hpp:6: C4_ENUM: MyEnum */
template<> const EnumPairs< MyEnum > enum_pairs()
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
/** enum: auto-generated from myenum.hpp:14: C4_ENUM: MyEnumClass */
template<> const EnumPairs< MyEnumClass > enum_pairs()
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