#pragma once

#include "freelist.h"
#include <cstring>
#include <mutex>
#include <sys/mman.h>
#include <unistd.h>

class malloc_allocator {
    static constexpr std::size_t heap_size = 64 * 1024;
    static constexpr std::size_t mmap_threshold = 4 * 1024;

    char heap_[heap_size];
    freelist fl_;
    mutable std::mutex mutex_;

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
        std::lock_guard<std::mutex> lock(mutex_);
        if (size == 0) return nullptr;
        if (size >= mmap_threshold) return mmap_alloc(size);
        return fl_.allocate(size);
    }

    void free(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!ptr) return;
        if (is_mmap_ptr(ptr)) {
            mmap_free(ptr, *reinterpret_cast<std::size_t*>(
                static_cast<char*>(ptr) - sizeof(std::size_t)));
            return;
        }
        fl_.deallocate(ptr);
    }

    void* realloc(void* ptr, std::size_t new_size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!ptr) {
            if (new_size == 0) return nullptr;
            if (new_size >= mmap_threshold) return mmap_alloc(new_size);
            return fl_.allocate(new_size);
        }
        if (new_size == 0) {
            if (is_mmap_ptr(ptr)) {
                mmap_free(ptr, *reinterpret_cast<std::size_t*>(
                    static_cast<char*>(ptr) - sizeof(std::size_t)));
            } else {
                fl_.deallocate(ptr);
            }
            return nullptr;
        }

        void* new_ptr;
        if (new_size >= mmap_threshold) {
            new_ptr = mmap_alloc(new_size);
        } else {
            new_ptr = fl_.allocate(new_size);
        }
        if (!new_ptr) return nullptr;

        std::size_t old_size;
        if (is_mmap_ptr(ptr)) {
            old_size = *reinterpret_cast<std::size_t*>(
                static_cast<char*>(ptr) - sizeof(std::size_t));
        } else {
            old_size = fl_.block_size_of(ptr);
            if (new_size <= old_size) {
                fl_.deallocate(new_ptr);
                return ptr;
            }
        }

        std::size_t copy = old_size < new_size ? old_size : new_size;
        std::memcpy(new_ptr, ptr, copy);

        if (is_mmap_ptr(ptr)) {
            mmap_free(ptr, old_size);
        } else {
            fl_.deallocate(ptr);
        }
        return new_ptr;
    }

    void* calloc(std::size_t nmemb, std::size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::size_t total = nmemb * size;
        if (size != 0 && total / size != nmemb) return nullptr;
        void* ptr;
        if (total >= mmap_threshold) {
            ptr = mmap_alloc(total);
        } else {
            ptr = fl_.allocate(total);
        }
        if (ptr) std::memset(ptr, 0, total);
        return ptr;
    }

    std::size_t used() {
        std::lock_guard<std::mutex> lock(mutex_);
        return fl_.used();
    }
};
