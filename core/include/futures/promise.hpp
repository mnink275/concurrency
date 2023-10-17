#pragma once

#include <memory>

#include "future.hpp"
#include "detail/pipe.hpp"

namespace ink {

template <class T>
class Promise final {
 public:
  Promise() : pipe_(std::make_shared<detail::Pipe<T>>()) {}

  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  Promise(Promise&&) noexcept = default;
  Promise& operator=(Promise&&) noexcept = default;

  ~Promise() = default;

  void SetValue(T value) {
    pipe_->SetValue(std::move(value));
  }

  void SetException(std::exception_ptr eptr) {
    pipe_->SetException(std::move(eptr));
  }

  Future<T> MakeFuture() {
    return Future<T>{pipe_};
  }

 private:
  std::shared_ptr<detail::Pipe<T>> pipe_;
};

}  // namespace ink
