#pragma once

#include <cstddef>
#include <thread>

#include "mpmc-queue.hpp"
#include "task.hpp"

namespace ink {

class KThreadPool {
 public:
  KThreadPool(std::size_t workers_amount) : workers_amount_(workers_amount) {
    InitWorkers();
  }
  ~KThreadPool() {
    for ([[maybe_unused]] auto&& worker : workers_) {
      Submit(Task{});
    }
    for (auto&& worker : workers_) {
      worker.join();
    }
  }

  void Submit(Task task) { queue_.Put(std::move(task)); }
  std::size_t GetWorkersAmount() { return workers_amount_; }

 private:
  void InitWorkers() {
    for (std::size_t i = 0; i < workers_amount_; ++i) {
      workers_.emplace_back([this]() {
        auto task = queue_.Fetch();
        if (!task) return;
        task();
      });
    }
  }

 private:
  std::size_t workers_amount_;
  std::vector<std::thread> workers_;
  MPMCUnboundedQueue<Task> queue_;
};

}  // namespace ink