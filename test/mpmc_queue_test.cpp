#include <gtest/gtest.h>

#include <core/include/mpmc_queue.hpp>

namespace ink::test {

struct MoveOnly {
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
};

TEST(MPMCQueue, MoveOnly) {
  MoveOnly move_only{};
  MPMCUnboundedQueue<MoveOnly> queue;
  queue.Put(std::move(move_only));
  auto result = queue.Fetch();
}

}  // namespace ink::test
