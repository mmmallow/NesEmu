cmake_minimum_required(VERSION 3.26)

project(nesemu)

set(CMAKE_CXX_STANDARD 17)

set (MY_SOURCES
    src/main.cpp
    src/CPU.cpp
    src/tests/InstructionsTest.cpp
)

add_executable(nesemu ${MY_SOURCES})
