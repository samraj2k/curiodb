cmake_minimum_required(VERSION 3.28)
project(curiodb)

set(CMAKE_CXX_STANDARD 20)

# Add Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(curiodb main.cpp
        headers/storage/page.h
        src/storage/page.cpp
        headers/storage/item.h
        headers/configs/constants.h
        headers/storage/buffer.h
        headers/storage/block.h
        headers/storage/lock.h
        src/storage/buffer.cpp
        tests/lock_test.cpp
        tests/lock_test.cpp
)

target_link_libraries(curiodb gtest gtest_main)
target_include_directories(curiodb PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})