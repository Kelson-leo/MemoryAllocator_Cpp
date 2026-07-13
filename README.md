# Memory Allocator — C++17

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14+-brightgreen.svg)](https://cmake.org/)
[![Tests](https://img.shields.io/badge/tests-21%2F21-green.svg)]()
[![Thread-safe](https://img.shields.io/badge/thread-safe-yes-green.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)]()

Custom memory allocators built from scratch in C++17: arena, pool, free list, and a
full `malloc`/`free` implementation. Part of my C++ portfolio focused on systems programming.

## Allocators

| Phase | Allocator | Highlights |
|---|---|---|
| 1 | **Arena** | Bump allocator, `std::align`, `reinterpret_cast`, reset semantics |
| 2 | **Pool** | Fixed-size blocks, embedded free list, O(1) alloc/free, `union` |
| 3 | **Free List** | Variable-size, first-fit, splitting, coallescing, boundary tags |
| 4 | **malloc** | `malloc`/`free`/`realloc`/`calloc`, `mmap`, overflow check |

All allocators are **thread-safe** (`std::mutex` + `std::lock_guard`).

## Quick Usage

```cpp
#include "arena.h"
#include "pool.h"
#include "malloc.h"

// Arena: bump allocator, reset to free all
char buf[1024];
arena<char> scratch(buf, sizeof(buf));
int* data = scratch.allocate(100);     // 400 bytes
scratch.reset();                       // "free" everything at once

// Pool: fixed-size blocks, O(1) alloc/free
struct Particle { float x, y, z; };
pool<Particle, 1000> particle_pool;
Particle* p = particle_pool.allocate(); // O(1)
particle_pool.deallocate(p);            // O(1), returns to free list

// malloc: full API with mmap for large blocks
malloc_allocator ma;
void* ptr = ma.malloc(8192);            // >= 4 KB → mmap
ptr = ma.realloc(ptr, 16384);           // grow + copy data
ma.free(ptr);

// All allocators are thread-safe
```

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
  test_concurrency.cpp - 2 tests
```
