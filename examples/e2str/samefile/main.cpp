#include <iostream>

#include "myenum.hpp"

template< typename E >
int test_e2str()
{
    int error_status = 0;
    for(auto &p : enum_pairs< E >())
    {
        // test a round trip
        bool ok = (str2e< E >(e2str(p.value)) == p.value);
        error_status |= !ok;
        // print something
        std::cout << e2str(p.value) << "=" << (int)str2e< E >(p.name)
                  << "  (" << (ok?"ok":"wrong") << ")" "\n";
    }
    return error_status;
}

int main(int argc, const char* argv[])
{
    return test_e2str< MyEnum >() | test_e2str< MyEnumClass >();
}

