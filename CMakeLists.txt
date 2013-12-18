project(bench)
cmake_minimum_required(VERSION 2.6)

add_definitions("-D_POSIX_C_SOURCE=200112L")
add_definitions("-D__STDC_FORMAT_MACROS=1")
add_definitions("-D__STDC_LIMIT_MACROS=1")
add_definitions("-D__STDC_CONSTANT_MACROS=1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DMSGPACK=1 -Wall -Wextra -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

find_path(TARANTOOL_INCLUDE_DIR
    NAMES box/box.h
    PATHS "../../src/"
)
find_path(TARANTOOL_BOX_INCLUDE_DIR
    NAMES request.h port.h
    PATH_SUFFIXES "box"
    PATHS "../../src"
)

if(TARANTOOL_INCLUDE_DIR AND TARANTOOL_BOX_INCLUDE_DIR)
        message(STATUS "Found Tarantool includes ${TARANTOOL_INCLUDE_DIR}")
        message(STATUS "Found Box includes ${TARANTOOL_BOX_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find Tarantool source tree")
endif()

include_directories(${TARANTOOL_INCLUDE_DIR})
include_directories(${TARANTOOL_INCLUDE_DIR}/lib)
include_directories(${TARANTOOL_BOX_INCLUDE_DIR})

# By default create Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE
        STRING "Choose the type of build, options are: Debug Release."
        FORCE)
endif()

set(BENCH_SRC_LIST
    bench.cc
)

add_library(${PROJECT_NAME} SHARED ${BENCH_SRC_LIST})
