# Memory Allocator — C++17 Portfolio Project

## Project identity

- **What:** `malloc`/`free` from scratch — arena, pool, free list allocators
- **Language:** C++17 (header-only library + test executable)
- **Why:** Most-asked C++ interview topic. Shows you understand what happens under `new`/`shared_ptr`.
- **Not:** A production allocator. It's a learning vehicle.
- **Architecture:** Simple library. Header-only, no interfaces, no hexagonal overengineering.

## What new agent sessions need to know

Read this first, then read `docs/PROGRESS.md` for current phase.
Read `docs/INTERVIEW_QA.md` for concepts to explain in interviews.
Read `docs/TECHNICAL_DEBT.md` for known issues.

## Teaching approach — how this project teaches C++

The user is learning C++ while building. Every code change should be accompanied by:
- Why this approach and not another (e.g., `reinterpret_cast` vs C-style cast)
- What the C++ standard says about this (e.g., strict aliasing, alignment rules)
- Common pitfalls and how we avoid them

### Key concepts to teach through this project

| Phase | Concept | When it comes up |
|---|---|---|
| Arena | Stack vs heap, pointer arithmetic | Implementing `allocate()` |
| Arena | `std::align`, `alignof`, `alignas` | Making allocations aligned |
| Arena | `reinterpret_cast` vs `static_cast` vs C-cast | Casting `char*` buffer to `T*` |
| Pool | Union, embedded data structures | Free list inside deallocated slots |
| Pool | `static_assert`, compile-time checks | Ensuring `sizeof(T) >= sizeof(void*)` |
| Free list | `sbrk` vs `mmap`, syscalls | Getting memory from OS |
| Free list | `std::bit_cast` (C++20) or memcpy | Type-punning safely |
| All | RAII in test fixtures | Test setup/teardown |
| All | Undefined behavior — strict aliasing, use-after-free, double free | Writing safe allocator code |
| All | AddressSanitizer, valgrind, UBSan | Debugging memory bugs |
| All | `-Wall -Wextra -Wpedantic` | Zero-warning policy |

### Terminology the user must learn

- **Alignment:** memory address must be multiple of N (e.g., `int` at address multiple of 4)
- **Padding:** unused bytes between struct members to satisfy alignment
- **Fragmentation (external):** free memory exists but split into small pieces, can't satisfy large request
- **Fragmentation (internal):** allocated block is larger than requested (padding/metadata)
- **Coallescing:** merging adjacent free blocks into one larger block
- **Splitting:** dividing a large free block to satisfy a small request
- **Bump allocator:** another name for arena (pointer bumps forward, never frees)
- **Embedded free list:** the linked list of free blocks lives INSIDE the free blocks themselves

### C++ mindset for this project

1. **Read the standard.** When in doubt about alignment rules, strict aliasing, or UB, check cppreference.
2. **Cast explicitly.** Never C-style cast `(T*)ptr`. Use `reinterpret_cast` (for pointer type changes) or `static_cast` (for related types).
3. **Compile-time over runtime.** Use `static_assert` to catch errors at compile time (wrong alignment, wrong size).
4. **Test edge cases.** What happens with `malloc(0)`? `free(nullptr)`? `realloc(ptr, 0)`?
5. **Tooling is your friend.** `valgrind ./tests`, `-fsanitize=address`, `-fsanitize=undefined`.
6. **Header-only with tests.** Library is `.h` files. Tests are `.cpp` files that `#include` them.

## Project structure

```
Systems/Allocator/
  src/
    arena.h           # Arena (bump) allocator
    pool.h            # Pool (fixed-size) allocator
    freelist.h        # Free list (variable-size) allocator
    malloc.h          # Full malloc/free/realloc/calloc
  tests/
    test_main.cpp     # Google Test runner
    test_arena.cpp
    test_pool.cpp
    test_freelist.cpp
    test_malloc.cpp
  docs/
    PROGRESS.md       # Current phase, what's done
    TECHNICAL_DEBT.md # Known issues, limitations
    INTERVIEW_QA.md   # Questions + answers for interviews
    CV_BULLETS.md     # Resume bullets for this project
  README.md
  CMakeLists.txt
  .gitignore
```

## Build & Tools

```bash
# Build and test
cmake -B build && cmake --build build && ./build/allocator_tests

# With sanitizers
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build

# Memory check
valgrind ./build/allocator_tests
```

## C++ rules for this project

- **No `new`/`delete` in library code.** The allocator IS the memory manager. Use raw buffers and placement new only where needed.
- **No C-style casts.** `reinterpret_cast` for pointer type changes, `static_cast` for sizes/offsets.
- **No `memcpy` for type-punning.** Use `std::bit_cast` (C++20) or explicit `reinterpret_cast` with proper alignment.
- **Header-only.** All allocator code in `.h` files. Tests in `.cpp`.
- **`static_assert` liberally.** Catch alignment/size bugs at compile time.
- **No `#include <iostream>` in library headers.** Use `assert` or return error codes. I/O is for tests/demos.
- **Zero warnings.** `-Wall -Wextra -Wpedantic -Werror` in CI/debug builds.

## Security & correctness awareness (junior level)

Things to check before marking any phase "done":
- [ ] What happens if user calls `free(ptr)` twice? (double free detection or documented UB)
- [ ] What happens if user passes wrong pointer to `free`? (not our allocator's pointer)
- [ ] Alignment: does `allocate<T>(n)` return a pointer valid for type `T`?
- [ ] Integer overflow: what if `size * n` overflows?
- [ ] `malloc(0)`: return `nullptr` or a unique pointer? (C standard says implementation-defined)
- [ ] `realloc(ptr, 0)`: equivalent to `free(ptr)`? (C standard says yes)

We won't solve all of these perfectly — this is junior level. But we document
limitations in `TECHNICAL_DEBT.md` and show awareness.

## Documentation discipline — CRITICAL

This is the most important rule in this file. Every agent session MUST maintain
the learning documentation. Without it, future sessions start blind.

### Primary document: `docs/LEARNING_BOOK.md`

One unified reference book organized by phases. This is THE document the user
studies from. Each phase must contain:

1. **Objective** — what this phase builds, in one sentence
2. **Final code** — the complete implementation for this phase
3. **Call flow** — entry point → internal calls → return, with explanation of each step
4. **Assembly x86 comparison** — when pointer arithmetic or low-level ops are involved,
   show the equivalent x86-64 asm so the user builds intuition
5. **Concept table** — summary of every new concept learned in this phase
6. **Limitations** — what this code does NOT handle (honesty is educational)

Style rules:
- Simple, technical, no forced analogies (the user is not a child)
- Contextualize WHY something exists, not just WHAT it does
- Show the bug/fix pattern when relevant ("first we tried X, but Y happens, so Z")

### Supporting documents

| File | When |
|---|---|
| `docs/LEARNING_BOOK.md` | **After every phase/feature** — add new section |
| `docs/PROGRESS.md` | After completing a phase — terse status update |
| `docs/INTERVIEW_QA.md` | When a concept could be an interview question |
| `docs/TECHNICAL_DEBT.md` | Known limitations, edge cases, security notes |
| `docs/CV_BULLETS.md` | When project has enough substance for resume |
| `docs/PO_PROMPT.md` | Static — prompt for external PO agent session |

### Rule for all future agent sessions

**Before writing any code**, read `docs/LEARNING_BOOK.md` and `docs/PROGRESS.md`
to know the current phase and what's been taught.

**After writing code**, update `docs/LEARNING_BOOK.md` with the new phase's
explanations, call flows, and assembly comparisons. Then update `docs/PROGRESS.md`
with current status.

This ensures the user can study from the book without replaying the entire
conversation history.

## Git discipline (same as raytracer)

- Commits in English, format: `type: description` (feat:, fix:, refactor:, docs:)
- No co-authors, no AI references
- One logical change per commit
- Tests alongside code (not after)

## Current phase

**Phase 0:** Project setup — directory structure, CMakeLists.txt, test runner, README.
**Next:** Phase 1 — Arena Allocator.
