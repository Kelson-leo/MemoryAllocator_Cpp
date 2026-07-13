#include <gtest/gtest.h>
#include "../src/malloc.h"

TEST(MallocTest, AllocAndFree) {
    malloc_allocator ma;

    void* p = ma.malloc(128);
    ASSERT_NE(p, nullptr);
    EXPECT_GT(ma.used(), 0);

    ma.free(p);
}

TEST(MallocTest, MallocZeroReturnsNull) {
    malloc_allocator ma;
    EXPECT_EQ(ma.malloc(0), nullptr);
}

TEST(MallocTest, FreeNullIsSafe) {
    malloc_allocator ma;
    ma.free(nullptr);  // must not crash
}

TEST(MallocTest, ReallocGrowCopiesData) {
    malloc_allocator ma;

    int* p = static_cast<int*>(ma.malloc(sizeof(int) * 4));
    p[0] = 42; p[1] = 99; p[2] = 7; p[3] = 13;

    int* q = static_cast<int*>(ma.realloc(p, sizeof(int) * 8));
    ASSERT_NE(q, nullptr);
    EXPECT_EQ(q[0], 42);
    EXPECT_EQ(q[1], 99);
    EXPECT_EQ(q[2], 7);
    EXPECT_EQ(q[3], 13);

    ma.free(q);
}

TEST(MallocTest, CallocZeroesMemory) {
    malloc_allocator ma;

    int* p = static_cast<int*>(ma.calloc(10, sizeof(int)));
    ASSERT_NE(p, nullptr);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(p[i], 0);
    }
    ma.free(p);
}

TEST(MallocTest, LargeAllocUsesMmap) {
    malloc_allocator ma;

    void* p = ma.malloc(8 * 1024);  // 8 KB > mmap_threshold (4 KB)
    ASSERT_NE(p, nullptr);
    ma.free(p);
}
