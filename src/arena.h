#pragma once

#include <cstddef>
#include <memory>
#include <mutex>

template <typename T>
class arena {
public:
    arena(T* buffer, std::size_t capacity_bytes)
        : buffer_(reinterpret_cast<char*>(buffer)),
          offset_(0),
          capacity_(capacity_bytes) {}

    T* allocate(std::size_t n) {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocate_unlocked(n);
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        offset_ = 0;
    }

    std::size_t used() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return offset_;
    }

    std::size_t capacity() const { return capacity_; }

private:
    T* allocate_unlocked(std::size_t n) {
        std::size_t size = n * sizeof(T);
        std::size_t space = capacity_ - offset_;
        void* ptr = buffer_ + offset_;

        if (!std::align(alignof(T), size, ptr, space)) return nullptr;

        offset_ = static_cast<char*>(ptr) - buffer_ + size;
        return static_cast<T*>(ptr);
    }

    char* buffer_;
    std::size_t offset_;
    std::size_t capacity_;
    mutable std::mutex mutex_;
};
