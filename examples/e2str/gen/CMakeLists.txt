cmake_minimum_required(VERSION 2.8)
project(e2str)

set(CMAKE_CXX_STANDARD 11)
include(regen.cmake)

set(SRC main.cpp enum.hpp test_enum.cpp)
set(RFL myenum.hpp) # files to be reflected

regen_setup(${PROJECT_SOURCE_DIR} GENH GENC GENT ${RFL})
add_executable(e2str ${GENH} ${GENC} ${RFL} ${SRC} regen.py)
add_dependencies(e2str ${GENT})
