#include "wait_group.hpp"

#include <condition_variable>
#include <cstdlib>
#include <mutex>

namespace ink {

void WaitGroup::Add(std::size_t count) {
  std::lock_guard lock(mutex_);
  slots_ += count;
}

void WaitGroup::Done() {
  std::lock_guard lock(mutex_);
  slots_ -= 1;
  if (slots_ == 0) free_slots_.notify_all();
}

void WaitGroup::Wait() {
  std::unique_lock lock(mutex_);
  free_slots_.wait(lock, [this]() { return slots_ == 0; });
}

}  // namespace ink