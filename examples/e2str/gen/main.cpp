
#include "test_enum.cpp" // that's right.
#include "myenum.gen.hpp"

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

    EXPECT_BM_EQ(esyms< MyEnumClass >().get("MyEnumClass::FOO")->value, MyEnumClass::FOO);
    EXPECT_BM_EQ(esyms< MyEnumClass >().get("FOO")->value, MyEnumClass::FOO);

    EXPECT_BM_EQ(esyms< MyBitmask >().get("BM_FOO")->value, MyBitmask::BM_FOO);
    EXPECT_BM_EQ(esyms< MyBitmask >().get("FOO")->value, MyBitmask::BM_FOO);
    EXPECT_BM_EQ(str2bm< MyBitmask >("BM_FOO|BM_BAR"), BM_FOO_BAR);
    EXPECT_BM_EQ(str2bm< MyBitmask >("FOO|BAR"), BM_FOO_BAR);

    EXPECT_BM_EQ(esyms< MyBitmaskClass >().get("MyBitmaskClass::BM_FOO")->value, MyBitmaskClass::BM_FOO);
    EXPECT_BM_EQ(esyms< MyBitmaskClass >().get("BM_FOO")->value, MyBitmaskClass::BM_FOO);
    EXPECT_BM_EQ(esyms< MyBitmaskClass >().get("FOO")->value, MyBitmaskClass::BM_FOO);
    EXPECT_BM_EQ(str2bm< MyBitmaskClass >("MyBitmaskClass::BM_FOO|MyBitmaskClass::BM_BAR"), MyBitmaskClass::BM_FOO_BAR);
    EXPECT_BM_EQ(str2bm< MyBitmaskClass >("BM_FOO|BM_BAR"), MyBitmaskClass::BM_FOO_BAR);
    EXPECT_BM_EQ(str2bm< MyBitmaskClass >("FOO|BAR"), MyBitmaskClass::BM_FOO_BAR);

    test_e2str< MyEnumClass >();
    test_e2str< MyEnum >();

    test_bm2str< MyBitmask >();
    test_bm2str< MyBitmaskClass >();

    return error_status;
}
