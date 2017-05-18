#include <cstdio>
#include <cstdlib>

#include "main.hpp"
#include "main.gen.hpp"

int main(int argc, char* argv[])
{
    printf("hello\n");
#define N 10

    int i = 1, ic = 10;
    int arr[N] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        arrc[N] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    TestStruct ts1{1, 2, 3}, ts2{4, 5, 6};
    c4::Archive< c4::ArchiveStreamText > txt;

    using arktype = c4::Archive< c4::ArchiveStreamBinary >;

    {
        txt.write_mode(true);
        txt("i", &ic);
        txt("arr", &arrc);
        txt("ts2", &ts2);
    }

    {
        arktype ark;
        FILE *output = fopen("archive.bin", "wb");
        ark.write_mode(true, output);
        ark("i", &i);
        ark("arr", &arr);
        ark("ts1", &ts1);
        fclose(output);
    }

    {
        arktype ark;
        FILE *input = fopen("archive.bin", "rb");
        ark.write_mode(false, input);
        ark("i", &ic);
        ark("arr", &arrc);
        ark("ts1", &ts2);
        fclose(input);
    }

    {
        txt.write_mode(true);
        txt("i", &ic);
        txt("arr", &arrc);
        txt("ts2", &ts2);
    }

    return 0;
}

