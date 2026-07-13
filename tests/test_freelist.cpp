#include <gtest/gtest.h>
#include "../src/freelist.h"

TEST(FreeListTest, AllocateAndUse) {
    char buf[4096];
    freelist fl(buf, sizeof(buf));

    void* p = fl.allocate(128);
    ASSERT_NE(p, nullptr);
    EXPECT_GT(fl.used(), 0);
    EXPECT_LT(fl.used(), sizeof(buf));
}

TEST(FreeListTest, ExhaustionReturnsNull) {
    char buf[512];
    freelist fl(buf, sizeof(buf));

    fl.allocate(400);  // takes most of the heap
    void* p = fl.allocate(400);  // too big for remaining
    EXPECT_EQ(p, nullptr);
}

TEST(FreeListTest, FreeAndReuse) {
    char buf[4096];
    freelist fl(buf, sizeof(buf));

    void* p1 = fl.allocate(256);
    std::size_t used_after_alloc = fl.used();

    fl.deallocate(p1);
    EXPECT_LT(fl.used(), used_after_alloc);

    void* p2 = fl.allocate(256);
    EXPECT_EQ(p1, p2);
}

TEST(FreeListTest, CoallesceAdjacentFrees) {
    char buf[4096];
    freelist fl(buf, sizeof(buf));

    void* a = fl.allocate(128);
    void* b = fl.allocate(128);
    std::size_t used_with_two = fl.used();

    fl.deallocate(a);
    fl.deallocate(b);

    void* c = fl.allocate(256);
    ASSERT_NE(c, nullptr);
    EXPECT_LE(fl.used(), used_with_two);
}

TEST(FreeListTest, AllocAfterDeallocMiddle) {
    char buf[4096];
    freelist fl(buf, sizeof(buf));

    fl.allocate(128);
    void* b = fl.allocate(128);
    fl.allocate(128);

    fl.deallocate(b);

    void* d = fl.allocate(64);
    ASSERT_NE(d, nullptr);
}
