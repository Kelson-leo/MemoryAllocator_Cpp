#include <gtest/gtest.h>
#include <thread>
#include <set>
#include <mutex>
#include <atomic>
#include "../src/arena.h"
#include "../src/pool.h"

TEST(ConcurrencyTest, ArenaMultiThreadAlloc) {
    char buf[4096];
    arena<char> a(buf, sizeof(buf));

    std::set<char*> pointers;
    std::mutex set_mutex;

    auto worker = [&]() {
        for (int i = 0; i < 50; ++i) {
            char* p = a.allocate(16);
            if (p) {
                std::lock_guard<std::mutex> lock(set_mutex);
                pointers.insert(p);
            }
        }
    };

    std::thread t1(worker);
    std::thread t2(worker);
    std::thread t3(worker);
    std::thread t4(worker);

    t1.join(); t2.join(); t3.join(); t4.join();

    EXPECT_EQ(pointers.size(), 4 * 50);
}

TEST(ConcurrencyTest, PoolMultiThreadAllocDealloc) {
    struct Data { int x; double y; };
    pool<Data, 100> p;

    std::atomic<int> count{0};

    auto worker = [&]() {
        for (int i = 0; i < 20; ++i) {
            Data* d = p.allocate();
            if (d) {
                d->x = 42;
                ++count;
                p.deallocate(d);
            }
        }
    };

    std::thread t1(worker);
    std::thread t2(worker);
    std::thread t3(worker);

    t1.join(); t2.join(); t3.join();

    EXPECT_GT(count, 0);
}
