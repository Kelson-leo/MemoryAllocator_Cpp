#pragma once

#include <cstddef>

template <typename T, std::size_t N>
class pool {
    static_assert(sizeof(T) >= sizeof(void*),
        "T too small for pool — pointer won't fit in free slot. Use arena instead.");

    union slot {
        T data;
        slot* next;
    };

public:
    pool() : free_list_(nullptr), available_(0) {
        for (std::size_t i = 0; i < N; ++i) {
            storage_[i].next = free_list_;
            free_list_ = &storage_[i];
            ++available_;
        }
    }

    T* allocate() {
        if (!free_list_) return nullptr;
        slot* s = free_list_;
        free_list_ = s->next;
        --available_;
        return &s->data;
    }

    void deallocate(T* ptr) {
        slot* s = reinterpret_cast<slot*>(ptr);
        s->next = free_list_;
        free_list_ = s;
        ++available_;
    }

    std::size_t available() const { return available_; }
    std::size_t capacity() const { return N; }

private:
    slot storage_[N];
    slot* free_list_;
    std::size_t available_;
};
