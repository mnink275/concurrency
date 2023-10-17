#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace ink {

template <class T>
class MPMCBlockingQueue final {
 public:
  MPMCBlockingQueue() = default;

  MPMCBlockingQueue(const MPMCBlockingQueue&) = delete;
  MPMCBlockingQueue& operator=(const MPMCBlockingQueue&) = delete;

  MPMCBlockingQueue(MPMCBlockingQueue&&) noexcept = delete;
  MPMCBlockingQueue& operator=(MPMCBlockingQueue&&) noexcept = delete;

  ~MPMCBlockingQueue() = default;

  void Put(T task) {
    std::lock_guard guard(mutex_);

    if (is_closed_) return;

    underlying_queue_.push(std::move(task));
    task_available_.notify_one();
  }

  std::optional<T> Fetch() {
    std::unique_lock lock(mutex_);

    task_available_.wait(
        lock, [this]() { return !underlying_queue_.empty() || is_closed_; });
    if (underlying_queue_.empty()) return std::nullopt;

    auto task = std::move(underlying_queue_.front());
    underlying_queue_.pop();

    return task;
  }

  void Close() {
    std::lock_guard guard(mutex_);

    is_closed_ = true;
    task_available_.notify_all();
  }

 private:
  std::queue<T> underlying_queue_;
  std::condition_variable task_available_;
  std::mutex mutex_;
  bool is_closed_{false};
};

}  // namespace ink
