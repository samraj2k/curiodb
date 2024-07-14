cmake_minimum_required(VERSION 3.28)
project(curiodb)

set(CMAKE_CXX_STANDARD 20)

add_executable(curiodb main.cpp
        headers/storage/page.h
        headers/storage/bplustree.h
        src/storage/page.cpp
        headers/storage/item.h
)
