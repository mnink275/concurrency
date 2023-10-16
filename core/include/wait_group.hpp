#pragma once

#include <condition_variable>
#include <cstdlib>
#include <mutex>

namespace ink {

class WaitGroup final {
 public:
  void Add(std::size_t count);
  void Done();
  void Wait();

 private:
  std::size_t slots_{0};
  std::condition_variable free_slots_;
  std::mutex mutex_;
};

}  // namespace ink