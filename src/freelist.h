#pragma once

#include <cstddef>

class freelist {
    struct header {
        std::size_t size_and_flags;
    };

    static constexpr std::size_t header_size = sizeof(header);
    static constexpr std::size_t footer_size = sizeof(std::size_t);
    static constexpr std::size_t min_block = header_size + footer_size + sizeof(void*);
    static constexpr std::size_t free_flag = 1;

    static std::size_t block_size(const header* h) {
        return h->size_and_flags & ~std::size_t{1};
    }

    static bool is_free(const header* h) {
        return h->size_and_flags & free_flag;
    }

    static std::size_t* footer_ptr(header* h) {
        return reinterpret_cast<std::size_t*>(
            reinterpret_cast<char*>(h) + header_size + block_size(h));
    }

    static void set_block(header* h, std::size_t size, bool free) {
        h->size_and_flags = size | (free ? free_flag : 0);
        *footer_ptr(h) = size;
    }

    static header* next_block(header* h) {
        return reinterpret_cast<header*>(
            reinterpret_cast<char*>(h) + header_size + block_size(h) + footer_size);
    }

    static header* left_neighbor(header* h) {
        std::size_t* left_ft = reinterpret_cast<std::size_t*>(
            reinterpret_cast<char*>(h) - footer_size);
        return reinterpret_cast<header*>(
            reinterpret_cast<char*>(h) - header_size - *left_ft - footer_size);
    }

    void merge_right(header* h) {
        header* right = next_block(h);
        set_block(h, block_size(h) + header_size + footer_size + block_size(right), true);
    }

    void coallesce(header* h) {
        header* right = next_block(h);
        bool right_free = (reinterpret_cast<char*>(right) < heap_ + capacity_) && is_free(right);
        if (right_free) merge_right(h);

        if (reinterpret_cast<char*>(h) > heap_) {
            header* left = left_neighbor(h);
            if (is_free(left)) merge_right(left);
        }
    }

    header* split(header* h, std::size_t requested) {
        std::size_t remaining = block_size(h) - requested - header_size - footer_size;
        if (remaining >= min_block) {
            set_block(h, requested, false);
            header* new_block = next_block(h);
            set_block(new_block, remaining, true);
            return new_block;
        }
        h->size_and_flags &= ~free_flag;
        return next_block(h);
    }

public:
    freelist(char* heap, std::size_t heap_size)
        : heap_(heap), capacity_(heap_size) {
        header* initial = reinterpret_cast<header*>(heap_);
        std::size_t usable = heap_size - header_size - footer_size;
        usable &= ~(alignof(std::max_align_t) - 1);
        set_block(initial, usable, true);
    }

    void* allocate(std::size_t size) {
        if (size < sizeof(void*)) size = sizeof(void*);
        size = (size + alignof(std::max_align_t) - 1) & ~(alignof(std::max_align_t) - 1);

        header* current = reinterpret_cast<header*>(heap_);
        header* end = reinterpret_cast<header*>(heap_ + capacity_);

        while (current < end) {
            if (is_free(current) && block_size(current) >= size) {
                split(current, size);
                return reinterpret_cast<char*>(current) + header_size;
            }
            current = next_block(current);
        }
        return nullptr;
    }

    void deallocate(void* ptr) {
        header* h = reinterpret_cast<header*>(
            reinterpret_cast<char*>(ptr) - header_size);
        h->size_and_flags |= free_flag;
        *footer_ptr(h) = block_size(h);
        coallesce(h);
    }

    std::size_t used() const {
        std::size_t total = 0;
        const header* current = reinterpret_cast<const header*>(heap_);
        const header* end = reinterpret_cast<const header*>(heap_ + capacity_);

        while (current < end) {
            if (!is_free(current)) total += block_size(current);
            current = reinterpret_cast<const header*>(
                reinterpret_cast<const char*>(current) + header_size + block_size(current) + footer_size);
        }
        return total;
    }

    std::size_t capacity() const { return capacity_; }

private:
    char* heap_;
    std::size_t capacity_;
};
