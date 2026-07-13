# Memory Allocator — C++17 Portfolio Project

## Project identity

- **What:** `malloc`/`free` from scratch — arena, pool, free list allocators
- **Language:** C++17 (header-only library + test executable)
- **Why:** Most-asked C++ interview topic. Shows you understand what happens under `new`/`shared_ptr`.
- **Not:** A production allocator. It's a learning vehicle.
- **Architecture:** Simple library. Header-only, no interfaces, no hexagonal overengineering.

## What new agent sessions need to know

Read these IN ORDER before writing any code:
1. `docs/PROGRESS.md` — current phase, what's done
2. `docs/learning/00_FUNDAMENTALS.md` — C++ concepts used throughout
3. `docs/learning/01_arena.md` — Phase 1 details (or latest phase file)

Then read `docs/TECHNICAL_DEBT.md` for known issues.
Read `docs/INTERVIEW_QA.md` for concepts to explain in interviews.

## CRITICAL: Documentation discipline

**Every time code changes, documentation MUST be updated in the same commit.**
No exceptions. A future agent session starts by reading docs, not code.

### Documentation structure

```
docs/
  learning/                    ← DIDACTIC MATERIAL (the "book")
    00_FUNDAMENTALS.md         ← C++ concepts: pragma once, templates, namespaces, architecture, mindset
    01_arena.md                ← Phase 1: arena allocator
    02_pool.md                 ← Phase 2: pool allocator (next)
    ...                        ← one file per phase, numbered
  PROGRESS.md                  ← current phase, completion log
  TECHNICAL_DEBT.md            ← known issues, limitations, security notes
  INTERVIEW_QA.md              ← per-phase Q&A for interview prep
  CV_BULLETS.md                ← resume bullets for this project
  PO_PROMPT.md                 ← static — prompt for external PO agent
```

### Rules for `docs/learning/` files

Each phase file must contain:

1. **Fluxo de arquivos (modularizacao)** — which file calls which, entry points, TDD flow.
   ASCII diagram showing: test file → header under test → dependencies.
   Example:
   ```
   tests/test_arena.cpp → src/arena.h → <memory> (std::align)
   ```
2. **Fluxo interno de chamadas** — entry point → function → sub-function → return.
   ASCII diagram for each key function.
3. **Final code** — complete implementation for the phase.
4. **Assembly x86 comparison** — when pointer arithmetic or low-level ops are involved.
5. **Concepts table** — summary of every new concept learned.
6. **Limitations** — what this code does NOT handle (honesty).

### Rules for teaching ("bizus e pulos do gato")

Every phase must teach:

| What the user must absorb | How to convey |
|---|---|
| **Sintaxe** — how to write it | Show the code |
| **Semantica** — what it means | Explain in 1-2 sentences |
| **Motivo** — why this and not another | "We use X because Y. If we used Z, this would happen..." |
| **Armadilha** — what breaks if you do it wrong | "Pegadinha:" or "Armadilha:" prefix |
| **Assembly** — how it becomes machine code | x86-64 snippet with explanation |
| **Entrevista** — what they'd ask about this | Goes in INTERVIEW_QA.md |

### Auto-update checklist (after every code change)

- [ ] `docs/learning/0X_*.md` — add new phase or update existing
- [ ] `docs/PROGRESS.md` — update current status
- [ ] `docs/INTERVIEW_QA.md` — add Q&A for new concepts
- [ ] `docs/TECHNICAL_DEBT.md` — add/remove issues as they're fixed/discovered
- [ ] `docs/CV_BULLETS.md` — update when project has new resume-worthy features

---

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
    learning/         # Didactic material (the "book")
    PROGRESS.md
    TECHNICAL_DEBT.md
    INTERVIEW_QA.md
    CV_BULLETS.md
    PO_PROMPT.md
  README.md
  CMakeLists.txt
  .gitignore
```

## Build & Tools

```bash
cmake -B build && cmake --build build && ./build/allocator_tests
valgrind ./build/allocator_tests
```

## C++ rules for this project

- **No `new`/`delete` in library code.** Raw buffers + placement new only where needed.
- **No C-style casts.** `reinterpret_cast` and `static_cast` only.
- **Header-only.** All allocator code in `.h`. Tests in `.cpp`.
- **`static_assert` liberally.** Catch alignment/size bugs at compile time.
- **Zero warnings.** `-Wall -Wextra -Wpedantic`.

## Git discipline

- Commits in English, format: `type: description` (feat:, fix:, refactor:, docs:)
- No co-authors, no AI references
- One logical change per commit
- Tests + docs in the same commit as the code they relate to

## Current phase

**Phase 1 done:** Arena allocator with `std::align`, 4 tests.
**Phase 2 next:** Pool allocator — fixed-size blocks, embedded free list, O(1).
