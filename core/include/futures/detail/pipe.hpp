#pragma once

#include <cstddef>
#include <memory>
#include <variant>
#include <condition_variable>
#include <mutex>

namespace ink::detail {

inline constexpr static std::size_t kValueId = 0;
inline constexpr static std::size_t kEPtrId = 1;
inline constexpr static std::size_t kEmptyId = 2;

template <typename T>
class Pipe final {
 public:
  using Result = std::variant<T, std::exception_ptr, std::monostate>;

 public:
  Pipe() : result_(std::in_place_index<kEmptyId>, std::monostate{}) {}

  Pipe(const Pipe&) = delete;
  Pipe& operator=(const Pipe&) = delete;

  Pipe(Pipe&&) = delete;
  Pipe& operator=(Pipe&&) = delete;

  ~Pipe() = default;

  Result WaitForResult() {
    std::unique_lock lock(mutex_);
    value_appeared_.wait(lock, [this](){ return result_.index() != kEmptyId; });

    return std::move(result_);
  }

  void SetValue(T value) {
    std::lock_guard guard(mutex_);
    result_.template emplace<kValueId>(std::move(value));
    value_appeared_.notify_all();
  }

  void SetException(std::exception_ptr eptr) {
    std::lock_guard guard(mutex_);
    result_.template emplace<kEPtrId>(std::move(eptr));
    value_appeared_.notify_all();
  }

 private:
  Result result_;
  std::condition_variable value_appeared_;
  std::mutex mutex_;
};

}  // namespace ink::detail
