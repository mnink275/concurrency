#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace ink {

template <class Task>
class MPMCUnboundedQueue {
 public:
  void Put(Task task) {
    std::lock_guard guard(mutex_);

    queue_.push(std::move(task));
    task_available_.notify_one();
  }

  Task Fetch() {
    std::unique_lock lock(mutex_);

    task_available_.wait(lock, [this]() { return !queue_.empty(); });
    auto task = std::move(queue_.front());
    queue_.pop();

    return task;
  }

 private:
  std::queue<Task> queue_;
  std::condition_variable task_available_;
  std::mutex mutex_;
};

}  // namespace ink
