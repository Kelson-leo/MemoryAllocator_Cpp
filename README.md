# Memory Allocator — C++17

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14+-brightgreen.svg)](https://cmake.org/)

Custom memory allocators built from scratch in C++17: arena, pool, free list, and a
full `malloc`/`free` implementation. Part of my C++ portfolio focused on systems programming.

## Allocators

- **Arena** — sequential bump allocator, reset all at once
- **Pool** — fixed-size block allocator, O(1) alloc/free, embedded free list
- **Free List** — variable-size allocator with first-fit, splitting, and coallescing
- **malloc** — full `malloc`/`free`/`realloc`/`calloc` backed by `mmap`/`sbrk`

## Build & Run

```bash
cmake -B build
cmake --build build
./build/allocator_tests
```

## Requirements

- C++17 compiler (g++ or clang++)
- CMake 3.14+
- Google Test (fetched automatically)

## Project Structure

```
src/
  arena.h       - Arena allocator
  pool.h        - Pool allocator
  freelist.h    - Free list allocator
  malloc.h      - Full malloc/free/realloc
tests/
  test_main.cpp - test runner
  test_arena.cpp
  test_pool.cpp
  test_freelist.cpp
  test_malloc.cpp
```
