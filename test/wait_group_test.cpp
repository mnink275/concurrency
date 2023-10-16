#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

#include <core/include/wait_group.hpp>
#include <utils/timer.hpp>

namespace ink::test {

using namespace std::chrono_literals;

TEST(WaitGroup, SimpleWork) {
  bool is_reached_{false};

  WaitGroup wg;
  wg.Add(5);
  for (int i = 0; i < 5; ++i) {
    wg.Done();
  }
  wg.Wait();

  is_reached_ = true;
  EXPECT_TRUE(is_reached_);
}

TEST(WaitGroup, Waiting) {
  WaitGroup wg;

  utils::Timer timer{};

  std::thread producer([&wg]() {
    std::this_thread::sleep_for(50ms);
    wg.Add(1);
    wg.Done();
  });

  wg.Wait();
  producer.join();

  EXPECT_TRUE(timer.GetElapsed() < 50ms + 20ms);
}

}  // namespace ink::test
