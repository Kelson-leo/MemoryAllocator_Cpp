# Memory Allocator — C++17

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14+-brightgreen.svg)](https://cmake.org/)
[![Tests](https://img.shields.io/badge/tests-19%2F19-green.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)]()

Custom memory allocators built from scratch in C++17: arena, pool, free list, and a
full `malloc`/`free` implementation. Part of my C++ portfolio focused on systems programming.

## Allocators

| Phase | Allocator | Highlights |
|---|---|---|
| 1 | **Arena** | Bump allocator, `std::align`, `reinterpret_cast`, reset semantics |
| 2 | **Pool** | Fixed-size blocks, embedded free list, O(1) alloc/free, `union` |
| 3 | **Free List** | Variable-size, first-fit, splitting, coallescing, boundary tags |
| 4 | **malloc** | `malloc`/`free`/`realloc`/`calloc`, `mmap` for large blocks, overflow check |

## Build & Run

```bash
cmake -B build
cmake --build build
./build/allocator_tests
```

Memory check:
```bash
valgrind ./build/allocator_tests
```

## Requirements

- C++17 compiler (g++ or clang++)
- CMake 3.14+
- Linux (uses `mmap`/`munmap` for large allocations)
- Google Test (fetched automatically via FetchContent)

## Project Structure

```
src/
  arena.h         - Arena (bump) allocator with alignment
  pool.h          - Pool allocator with embedded free list
  freelist.h      - Free list with first-fit, splitting, coallescing
  malloc.h        - malloc/free/realloc/calloc with mmap backend
tests/
  test_main.cpp   - test runner
  test_arena.cpp  - 4 tests
  test_pool.cpp   - 4 tests
  test_freelist.cpp - 5 tests
  test_malloc.cpp   - 6 tests
```
