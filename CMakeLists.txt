cmake_minimum_required(VERSION 3.28)
project(curiodb)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
)

FetchContent_MakeAvailable(googletest spdlog)

enable_testing()

add_executable(curiodb main.cpp
        headers/storage/page.h
        src/storage/page.cpp
        headers/storage/item.h
        headers/configs/constants.h
        headers/storage/buffer.h
        headers/storage/block.h
        headers/lock/lock.h
        src/storage/buffer.cpp
        tests/lock_test.cpp
        tests/lock_test.cpp
        headers/io/io.h
        src/io/io.cpp
        headers/io/bgwriter.h
        src/io/bgwriter.cpp
)

target_link_libraries(curiodb gtest gtest_main spdlog::spdlog)
include_directories(${CMAKE_SOURCE_DIR}/headers)
target_include_directories(curiodb PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})