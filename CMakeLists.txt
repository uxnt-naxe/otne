cmake_minimum_required(VERSION 3.16)
project(main C CXX)

# MSVC utf-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

# GNU utf-8
add_compile_options("$<$<C_COMPILER_ID:GNU>:-fexec-charset=GBK>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-fexec-charset=GBK>")
add_compile_options("$<$<C_COMPILER_ID:GNU>:-finput-charset=utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-finput-charset=utf-8>")

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "-m32")
#
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_FLAGS "-m32")

if (WIN32)
    message(STATUS "Building on Windows platform.")
elseif (APPLE)
    message(STATUS "Building on MacOS platform.")
    add_compile_definitions(TARGET_OS_MAC)
elseif (UNIX)
    message(STATUS "Building on UNIX-like OS platform.")
endif ()
