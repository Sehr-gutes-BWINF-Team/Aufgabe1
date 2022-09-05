cmake_minimum_required(VERSION 3.23)
project(Aufgabe1)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_STANDARD 20)

add_executable(Aufgabe1
        main.cpp)
