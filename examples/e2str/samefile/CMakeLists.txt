cmake_minimum_required(VERSION 2.8)
project(e2str)

include(codegen.cmake)

set(RFL myenum.hpp) # files to be reflected
set(SRC main.cpp)

setup_codegen_samefile(${PROJECT_SOURCE_DIR} GENH GENC ${RFL})
add_executable(e2str ${GENH} ${GENC} ${RFL} ${SRC})

