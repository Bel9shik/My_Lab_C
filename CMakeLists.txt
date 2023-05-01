cmake_minimum_required(VERSION 3.23)
project(My_Lab_C C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(My_Lab_C
        main.c
        testlab12.c)
