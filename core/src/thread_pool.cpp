#include "thread_pool.hpp"

#include <cstddef>
#include <thread>

namespace ink {

KThreadPool::KThreadPool(std::size_t workers_amount)
    : workers_amount_(workers_amount) {
  InitWorkers();
}

KThreadPool::~KThreadPool() {
  for ([[maybe_unused]] auto&& worker : workers_) {
    Submit(Task{});
  }
  for (auto&& worker : workers_) {
    worker.join();
  }
}

void KThreadPool::Submit(Task task) { queue_.Put(std::move(task)); }

std::size_t KThreadPool::GetWorkersAmount() { return workers_amount_; }

void KThreadPool::InitWorkers() {
  for (std::size_t i = 0; i < workers_amount_; ++i) {
    workers_.emplace_back([this]() {
      auto task = queue_.Fetch();
      if (!task) return;
      task();
    });
  }
}

}  // namespace ink
