cmake_minimum_required(VERSION 3.28)
project(curiodb)

set(CMAKE_CXX_STANDARD 20)

add_executable(curiodb main.cpp
        headers/storage/page.h
        src/storage/page.cpp
        headers/storage/item.h
        headers/configs/constants.h
        headers/storage/buffer.h
        headers/storage/block.h
        headers/storage/lock.h
        src/storage/buffer.cpp
)
