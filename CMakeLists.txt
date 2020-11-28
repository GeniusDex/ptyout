cmake_minimum_required(VERSION 3.10)
project(ptyout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES src/forkpty.cpp src/vtbuffer.cpp)

add_executable(ptyout ${SOURCES})
target_link_libraries(ptyout util)
