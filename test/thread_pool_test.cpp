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

TEST(ThreadPool, Capacity) {
  ThreadPool tp{5};
  EXPECT_EQ(tp.GetWorkersAmount(), 5);
}

TEST(ThreadPool, StandaloneThread) {
  std::string greeting = "Hello";

  {
    ThreadPool tp{1};
    tp.Submit([&greeting]() { greeting.append(", world!"); });
  }

  EXPECT_EQ(greeting, "Hello, world!");
}

TEST(ThreadPool, Parallel) {
  const std::size_t tasks_amount = 6;
  ThreadPool tp{tasks_amount};

  utils::Timer timer{};

  for (std::size_t i = 0; i < tasks_amount; i++) {
    tp.Submit([]() {
      // heavy calculations
      std::this_thread::sleep_for(1s);
    });
  }

  EXPECT_TRUE(timer.GetElapsed() < 1s + 500ms);
}

TEST(ThreadPool, WaitIdle) {
  ThreadPool tp{6};

  std::atomic<int> count{0};
  const std::size_t expected = 1'000;
  for (std::size_t i = 0; i < expected; i++) {
    tp.Submit([&count]() { count.fetch_add(1); });
  }

  tp.WaitIdle();
  EXPECT_EQ(expected, count.load());
}

}  // namespace ink::test
