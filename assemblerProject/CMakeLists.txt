cmake_minimum_required(VERSION 3.25)
project(assemblerProject C)

set(CMAKE_C_STANDARD 11)

add_executable(assemblerProject main.c assemble.h)
