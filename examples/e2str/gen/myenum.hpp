#ifndef _MYENUM_HPP_
#define _MYENUM_HPP_

#include "enum.hpp"
#define C4_ENUM()

C4_ENUM() // tag this enum for extraction
typedef enum {
  FOO = 0,
  BAR,
  BAZ,
  BOD,
} MyEnum;

C4_ENUM() // tag this enum for extraction
enum class MyEnumClass {
  FOO = 0,
  BAR,
  BAZ,
  BOD
};

#endif // !_MYENUM_HPP_
