#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

#include <core/include/mpmc_queue.hpp>
#include <utils/timer.hpp>

namespace ink::test {

using namespace std::chrono_literals;

struct MoveOnly {
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
};

TEST(MPMCQueue, MoveOnly) {
  MoveOnly move_only{};
  MPMCUnboundedBlockingQueue<MoveOnly> queue;
  queue.Put(std::move(move_only));
  auto result = queue.Fetch();
}

TEST(MPMCQueue, FIFO) {
  MPMCUnboundedBlockingQueue<std::size_t> queue;
  for (std::size_t i = 0; i < 1'000'000; ++i) {
    queue.Put(i);
  }
  for (std::size_t i = 0; i < 1'000'000; ++i) {
    EXPECT_EQ(queue.Fetch(), i);
  }
}

TEST(MPMCQueue, NoDeadlockClose) {
  MPMCUnboundedBlockingQueue<std::size_t> queue;

  utils::Timer timer{};

  std::thread consumer{[&queue]() { queue.Fetch(); }};

  std::this_thread::sleep_for(100ms);
  queue.Close();
  consumer.join();

  EXPECT_TRUE(timer.GetElapsed() < 200ms);
}

TEST(MPMCQueue, AfterClose) {
  MPMCUnboundedBlockingQueue<std::size_t> queue;
  queue.Put(5);

  std::thread consumer{[&queue]() {
    std::this_thread::sleep_for(100ms);
    queue.Put(10);
  }};

  queue.Close();
  consumer.join();

  {
    auto wrapper = queue.Fetch();
    EXPECT_TRUE(wrapper.has_value());
    EXPECT_EQ(wrapper.value(), 5);
  }
  {
    auto wrapper = queue.Fetch();
    EXPECT_FALSE(wrapper.has_value());
  }
}

}  // namespace ink::test
