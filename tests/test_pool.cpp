#include <gtest/gtest.h>
#include "../src/pool.h"

struct Data {
    int x;
    double y;
};

TEST(PoolTest, AllocateAllBlocks) {
    pool<Data, 10> p;

    for (int i = 0; i < 10; ++i) {
        Data* d = p.allocate();
        ASSERT_NE(d, nullptr);
    }

    EXPECT_EQ(p.available(), 0);
}

TEST(PoolTest, ExhaustionReturnsNull) {
    pool<Data, 3> p;

    p.allocate();
    p.allocate();
    p.allocate();
    Data* d = p.allocate();

    EXPECT_EQ(d, nullptr);
}

TEST(PoolTest, DeallocateAndReuse) {
    pool<Data, 5> p;

    Data* d1 = p.allocate();
    p.deallocate(d1);
    Data* d2 = p.allocate();

    EXPECT_EQ(d1, d2);
}

TEST(PoolTest, DeallocateMultipleReuseInOrder) {
    pool<Data, 4> p;

    Data* a = p.allocate();
    Data* b = p.allocate();
    p.allocate();

    p.deallocate(b);
    p.deallocate(a);

    Data* x = p.allocate();
    Data* y = p.allocate();

    EXPECT_EQ(x, a);
    EXPECT_EQ(y, b);
}
