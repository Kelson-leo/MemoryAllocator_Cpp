#include <gtest/gtest.h>
#include "../src/arena.h"

TEST(ArenaTest, AllocateSingleObject) {
    char buf[1024];
    arena<char> a(buf, sizeof(buf));

    char* p = a.allocate(1);
    ASSERT_NE(p, nullptr);
    EXPECT_GE(a.used(), sizeof(char));
}

TEST(ArenaTest, AllocateMultipleObjects) {
    int buf[100];
    arena<int> a(buf, sizeof(buf));

    int* p = a.allocate(10);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(a.used(), 10 * sizeof(int));
}

TEST(ArenaTest, ResetReusesMemory) {
    char buf[64];
    arena<char> a(buf, sizeof(buf));

    char* p1 = a.allocate(32);
    a.reset();
    char* p2 = a.allocate(32);

    EXPECT_EQ(p1, p2);
}

TEST(ArenaTest, ExhaustionReturnsNull) {
    char buf[16];
    arena<char> a(buf, sizeof(buf));

    char* p1 = a.allocate(16);
    ASSERT_NE(p1, nullptr);

    char* p2 = a.allocate(1);
    EXPECT_EQ(p2, nullptr);
}
