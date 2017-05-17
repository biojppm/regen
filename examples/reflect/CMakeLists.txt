cmake_minimum_required(VERSION 2.8)
project(reflect)

set(CMAKE_CXX_STANDARD 11)
include(regen.cmake)

set(SRC main.hpp main.cpp enum.hpp util.hpp serialize.hpp imgui.hpp)
set(RFL main.hpp) # files to be reflected

regen_setup(${PROJECT_SOURCE_DIR} GENH GENC GENT ${RFL})
add_executable(reflect ${GENH} ${GENC} ${SRC} regen.py)
if(GENT)
    add_dependencies(reflect ${GENT})
endif()
