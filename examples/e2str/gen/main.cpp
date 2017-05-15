#include <iostream>

#include "myenum.hpp"
#include "myenum.gen.hpp"

template< typename E >
int test_e2str()
{
    int error_status = 0;
    for(auto &p : esyms< E >())
    {
        bool ok = true;
        // test a round trip
        ok &= (str2e< E >(e2str(p.value)) == p.value);
        // test the other way around
        ok &= (strcmp(e2str(str2e< E >(p.name)), p.name) == 0);
        // report back
        error_status |= !ok;
        // print something
        std::cout << e2str(p.value) << "=" << (ssize_t)str2e< E >(p.name)
                  << "  (" << (ok?"ok":"wrong") << ")\n";
    }
    return error_status;
}

int main(int argc, const char* argv[])
{
    return test_e2str< MyEnum >() | test_e2str< MyEnumClass >();
}

