cmake_minimum_required(VERSION 2.8)
project(e2str)

set(CMAKE_CXX_STANDARD 11)
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../common")
list(APPEND CMAKE_MODULE_PATH "${COMMON_DIR}/cmake")
include(regen)
include_directories(${COMMON_DIR}/include)

set(SRC main.cpp)
set(RFL myenum.hpp) # files to be reflected

regen_setup(${PROJECT_SOURCE_DIR} GENH GENC GENT ${RFL})
add_executable(e2str ${GENH} ${GENC} ${RFL} ${SRC})
add_dependencies(e2str ${GENT})
