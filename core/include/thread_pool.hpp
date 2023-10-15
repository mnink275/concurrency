#pragma once

#include <cstddef>
#include <thread>
#include <vector>

#include "mpmc_queue.hpp"
#include "task.hpp"

namespace ink {

class KThreadPool {
 public:
  KThreadPool(std::size_t workers_amount);
  ~KThreadPool();

  void Submit(Task task);
  std::size_t GetWorkersAmount();

 private:
  void InitWorkers();

 private:
  std::size_t workers_amount_;
  std::vector<std::thread> workers_;
  MPMCUnboundedQueue<Task> queue_;
};

}  // namespace ink
