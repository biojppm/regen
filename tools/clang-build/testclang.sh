#!/bin/bash

clang=$1
[ -z "$clang" ] && ( echo "no clang was given" ; exit 1 )

function error()
{
    echo "$clang: $*"
    exit 1
}

function runtest()
{
    test=$1
    expected=$2
    $clang $test.cpp -o build/$test || error "could not compile $test"
    ./build/$test
    ret=$?
    if [ $ret != $expected ] ; then
        error "fail $test: expected $expected, got $ret"
    else
        echo "$clang: $test ok"
    fi
}

[ ! -d build ] && mkdir build
runtest testnolib 2
runtest testwithlib 3
runtest testwithcpplib 4
