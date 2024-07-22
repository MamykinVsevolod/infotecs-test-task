cmake_minimum_required(VERSION 3.10)

project(Program2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(program2 main.cpp)

target_link_libraries(program2 pthread)
