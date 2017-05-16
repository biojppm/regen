
#include "myenum.gen.hpp"
#include "test.cpp" // that's right.

int main(int argc, const char* argv[])
{
    EXPECT_EQ(eoffs_cls< MyEnum >(), 0);
    EXPECT_EQ(eoffs_pfx< MyEnum >(), 0);

    EXPECT_EQ(eoffs_cls< MyEnumClass >(), strlen("MyEnumClass::"));
    EXPECT_EQ(eoffs_pfx< MyEnumClass >(), 0);

    EXPECT_EQ(eoffs_cls< MyBitmask >(), 0);
    EXPECT_EQ(eoffs_pfx< MyBitmask >(), strlen("BM_"));

    EXPECT_EQ(eoffs_cls< MyBitmaskClass >(), strlen("MyBitmaskClass::"));
    EXPECT_EQ(eoffs_pfx< MyBitmaskClass >(), strlen("MyBitmaskClass::BM_"));

    EXPECT_EQ((int)esyms< MyEnumClass >().get("MyEnumClass::FOO")->value, (int)MyEnumClass::FOO);
    EXPECT_EQ((int)esyms< MyEnumClass >().get("FOO")->value, (int)MyEnumClass::FOO);

    EXPECT_EQ((int)esyms< MyBitmask >().get("BM_FOO")->value, (int)MyBitmask::BM_FOO);
    EXPECT_EQ((int)esyms< MyBitmask >().get("FOO")->value, (int)MyBitmask::BM_FOO);

    EXPECT_EQ((int)esyms< MyBitmaskClass >().get("MyBitmaskClass::BM_FOO")->value, (int)MyBitmaskClass::BM_FOO);
    EXPECT_EQ((int)esyms< MyBitmaskClass >().get("BM_FOO")->value, (int)MyBitmaskClass::BM_FOO);
    EXPECT_EQ((int)esyms< MyBitmaskClass >().get("FOO")->value, (int)MyBitmaskClass::BM_FOO);

    test_e2str< MyEnumClass >();
    test_e2str< MyEnum >();

    test_bm2str< MyBitmask >();
    test_bm2str< MyBitmaskClass >();

    return error_status;
}
