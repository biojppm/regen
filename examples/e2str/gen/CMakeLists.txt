cmake_minimum_required(VERSION 2.8)
project(e2str)

include(codegen.cmake)

set(SRC main.cpp)
set(RFL myenum.hpp main.cpp) # files to be reflected

setup_codegen(${PROJECT_SOURCE_DIR} GENH GENC ${RFL})
add_executable(e2str ${GENH} ${GENC} ${RFL} ${SRC})

