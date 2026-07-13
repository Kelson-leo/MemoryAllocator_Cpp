#pragma once

#include "freelist.h"
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

class malloc_allocator {
    static constexpr std::size_t heap_size = 64 * 1024;
    static constexpr std::size_t mmap_threshold = 4 * 1024;

    char heap_[heap_size];
    freelist fl_;

    static void* mmap_alloc(std::size_t size) {
        std::size_t total = size + sizeof(std::size_t);
        void* ptr = mmap(nullptr, total, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) return nullptr;
        *static_cast<std::size_t*>(ptr) = size;
        return static_cast<char*>(ptr) + sizeof(std::size_t);
    }

    static void mmap_free(void* ptr, std::size_t size) {
        munmap(reinterpret_cast<char*>(ptr) - sizeof(std::size_t),
               size + sizeof(std::size_t));
    }

    bool is_mmap_ptr(const void* ptr) const {
        return ptr < static_cast<const void*>(heap_) ||
               ptr >= static_cast<const void*>(heap_ + heap_size);
    }

public:
    malloc_allocator() : fl_(heap_, heap_size) {}

    void* malloc(std::size_t size) {
        if (size == 0) return nullptr;
        if (size >= mmap_threshold) return mmap_alloc(size);
        return fl_.allocate(size);
    }

    void free(void* ptr) {
        if (!ptr) return;
        if (is_mmap_ptr(ptr)) {
            mmap_free(ptr, *reinterpret_cast<std::size_t*>(
                static_cast<char*>(ptr) - sizeof(std::size_t)));
            return;
        }
        fl_.deallocate(ptr);
    }

    void* realloc(void* ptr, std::size_t new_size) {
        if (!ptr) return malloc(new_size);
        if (new_size == 0) { free(ptr); return nullptr; }

        void* new_ptr = malloc(new_size);
        if (!new_ptr) return nullptr;

        std::size_t old_size;
        if (is_mmap_ptr(ptr)) {
            old_size = *reinterpret_cast<std::size_t*>(
                static_cast<char*>(ptr) - sizeof(std::size_t));
        } else {
            old_size = fl_.block_size_of(ptr);
            if (new_size <= old_size) {
                free(new_ptr);
                return ptr;
            }
        }

        std::size_t copy = old_size < new_size ? old_size : new_size;
        std::memcpy(new_ptr, ptr, copy);
        free(ptr);
        return new_ptr;
    }

    void* calloc(std::size_t nmemb, std::size_t size) {
        std::size_t total = nmemb * size;
        if (size != 0 && total / size != nmemb) return nullptr;
        void* ptr = malloc(total);
        if (ptr) std::memset(ptr, 0, total);
        return ptr;
    }

    std::size_t used() const { return fl_.used(); }
};
