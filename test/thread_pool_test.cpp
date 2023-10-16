#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

#include <core/include/thread_pool.hpp>
#include <utils/timer.hpp>

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

  utils::Timer timer{};

  for (int i = 0; i < tasks_amount; i++) {
    tp.Submit([]() {
      // heavy calculations
      std::this_thread::sleep_for(1s);
    });
  }

  EXPECT_TRUE(timer.GetElapsed() < 1s + 500ms);
}

TEST(KThreadPool, WaitIdle) {
  KThreadPool tp{6};

  std::atomic<int> count{0};
  const int expected = 1'000;
  for (int i = 0; i < expected; i++) {
    tp.Submit([&count]() { count.fetch_add(1); });
  }

  tp.WaitIdle();
  EXPECT_EQ(expected, count.load());
}

}  // namespace ink::test
