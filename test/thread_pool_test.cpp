#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

#include <core/include/thread_pool.hpp>

namespace ink::test {

using namespace std::chrono_literals;

TEST(KThreadPool, Capacity) {
  KThreadPool tp{5};
  EXPECT_EQ(tp.GetWorkersAmount(), 5);
}

TEST(KThreadPool, StandaloneThread) {
  std::string greeting = "Hello";

  {
    KThreadPool tp{1};
    tp.Submit([&greeting]() { greeting.append(", world!"); });
  }

  EXPECT_EQ(greeting, "Hello, world!");
}

TEST(KThreadPool, Parallel) {
  const std::size_t tasks_amount = 6;
  KThreadPool tp{tasks_amount};

  const auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < tasks_amount; i++) {
    tp.Submit([]() {
      // heavy calculations
      std::this_thread::sleep_for(1s);
    });
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double, std::milli> elapsed = end - start;

  EXPECT_TRUE(elapsed < 1s + 500ms) << "Current value is: " << elapsed.count();
}

TEST(KThreadPool, Atomic) {
  KThreadPool tp{6};

  std::atomic<int> count{0};
  const int expected = 120;
  for (int i = 0; i < expected; i++) {
    tp.Submit([&count]() { 
      std::this_thread::sleep_for(30ms);
      count.fetch_add(1);
    });
  }

  std::this_thread::sleep_for(1s);
  EXPECT_EQ(expected, count.load());
}

}  // namespace ink::test
