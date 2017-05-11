#include <iostream>
#include "enum.hpp"
#include "foobar.hpp"

template< typename E >
int test_esyms()
{
    int error_status = 0;
    for(auto &p : esyms< E >())
    {
        // test a round trip
        bool ok = (str2e< E >(e2str(p.value)) == p.value);
        error_status |= !ok;
        // print something
        c4_log << e2str(p.value) << "=" << (ssize_t)str2e< E >(p.name)
               << " " << e2stroffs< E >(p.value)
               << "  (" << (ok?"ok":"wrong") << ")\n";
    }
    return error_status;
}

#include <atomic>
#include <thread>

int main(int argc, char *argv[])
{
    c4_log << "hello\n";
    auto CH_FONIX = c4_log.add_channel("fonix");
    c4_log(CH_FONIX) << "caralho\n";

    {
        auto CH1 = c4_log.add_channel("CH1");
        auto CH2 = c4_log.add_channel("CH2");
        std::atomic_bool t1ready(false), t2ready(false);
        auto t1 = std::thread([&CH1, &t1ready, &t2ready]{
            t1ready = true;
            while(!t2ready) C4_KEEP_EMPTY_LOOP;
            for(auto i = 0; i < 25; ++i)
            {
                c4_log(CH1) << "t1 diz ola!\n";
                c4::busy_wait(100.);
            }
        });
        auto t2 = std::thread([&CH2, &t1ready, &t2ready]{
            t2ready = true;
            while(!t1ready) C4_KEEP_EMPTY_LOOP;
            for(auto i = 0; i < 25; ++i)
            {
                c4_log(CH2) << "t2 sayz hello!\n";
                c4::busy_wait(105.);
            }
        });
        t1.join();
        t2.join();
    }
    assert(a::var == 1);
    assert(a::b::var == 2);
    assert(a::b::c::var == 3);
#define PRN_STRUCT_OFFSETS_(structure, field) C4_LOG(C4_XQUOTE(structure) ":" C4_XQUOTE(field)" - offset=%zu\n", offsetof(structure, field))
#define PRN_STRUCT_OFFSETS(field) PRN_STRUCT_OFFSETS_(a::b::c::Example, field)
C4_FOR_EACH(PRN_STRUCT_OFFSETS, a, b, c);
C4_FOR_EACH(PRN_STRUCT_OFFSETS, a);
//    PRN_STRUCT_OFFSETS(a);
    ssize_t stat = 0;
    stat |= test_esyms< FooBar1 >();
    stat |= test_esyms< FooBar2 >();
    stat |= test_esyms< FooBar3 >();
    stat |= test_esyms< FooBar4 >();
    stat |= test_esyms< FooBar5 >();
    stat |= test_esyms< FooBar6 >();
    return stat;
}
