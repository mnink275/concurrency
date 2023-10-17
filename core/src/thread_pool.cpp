#include "thread_pool.hpp"

#include <cstddef>
#include <optional>
#include <thread>

namespace ink {

ThreadPool::ThreadPool(std::size_t workers_amount)
    : workers_amount_(workers_amount) {
  InitWorkers();
}

ThreadPool::~ThreadPool() {
  queue_.Close();
  for (auto&& worker : workers_) {
    worker.join();
  }
}

void ThreadPool::Submit(Task task) {
  tasks_.Add(1);
  queue_.Put(std::move(task));
}

std::size_t ThreadPool::GetWorkersAmount() const { return workers_amount_; }

void ThreadPool::WaitIdle() { tasks_.Wait(); }

void ThreadPool::InitWorkers() {
  for (std::size_t i = 0; i < workers_amount_; ++i) {
    workers_.emplace_back([this]() {
      for (;;) {
        auto task_wrapper = queue_.Fetch();
        if (!task_wrapper.has_value()) return;

        auto task = std::move(task_wrapper.value());
        task();

        tasks_.Done();
      }
    });
  }
}

}  // namespace ink
