cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(cbench C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find MsgPuck library
set(MsgPuck_FIND_REQUIRED ON)
find_package(MsgPuck)
include_directories(${MSGPUCK_INCLUDE_DIRS})

# Set CFLAGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra")

# Build module
add_subdirectory(cbench)
