cmake_minimum_required(VERSION 3.10)

project(Program1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(program1 main.cpp)

target_link_libraries(program1 pthread)
