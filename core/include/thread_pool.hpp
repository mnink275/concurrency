#pragma once

#include <cstddef>
#include <thread>
#include <vector>

#include "mpmc_queue.hpp"
#include "task.hpp"
#include "wait_group.hpp"
namespace ink {

class ThreadPool final {
 public:
  ThreadPool(std::size_t workers_amount);

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  ThreadPool(ThreadPool&&) noexcept = delete;
  ThreadPool& operator=(ThreadPool&&) noexcept = delete;

  ~ThreadPool();

  void Submit(Task task);
  std::size_t GetWorkersAmount() const;
  void WaitIdle();

 private:
  void InitWorkers();

 private:
  std::size_t workers_amount_;
  std::vector<std::thread> workers_;
  MPMCBlockingQueue<Task> queue_;
  WaitGroup tasks_;
};

}  // namespace ink
