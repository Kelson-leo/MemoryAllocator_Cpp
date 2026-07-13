cmake_minimum_required(VERSION 3.14)
project(MemoryAllocator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(allocator_tests
    tests/test_main.cpp
)

target_link_libraries(allocator_tests gtest_main pthread)
target_compile_options(allocator_tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME UnitTests COMMAND allocator_tests)
