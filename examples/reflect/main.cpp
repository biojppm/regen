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
    c4::Archive< c4::ArchiveStreamText > a;

    {
        FILE *output = fopen("archive.txt", "w");
        a.write_mode(true, output);
        a("i", &i);
        a("arr", &arr);
        a("ts1", &ts1);
        fclose(output);
    }

    {
        a.write_mode(true);
        a("i", &ic);
        a("arr", &arrc);
        a("ts2", &ts2);
    }

    {
        FILE *input = fopen("archive.txt", "r");
        a.write_mode(false, input);
        a("i", &ic);
        a("arr", &arrc);
        a("ts1", &ts2);
        fclose(input);
    }

    {
        a.write_mode(true);
        a("i", &ic);
        a("arr", &arrc);
        a("ts2", &ts2);
    }

    return 0;
}

