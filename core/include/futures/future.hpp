#pragma once

#include <memory>
#include <variant>

#include "detail/pipe.hpp"

namespace ink {

template <class T>
class Future final {
 public:
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  Future(Future&&) noexcept = default;
  Future& operator=(Future&&) noexcept = default;

  ~Future() = default;

  T GetValue() const {
    auto result = pipe_->WaitForResult();

    if (result.index() == detail::kEPtrId) {
      std::rethrow_exception(std::get<detail::kEPtrId>(std::move(result)));
    }

    return std::get<detail::kValueId>(std::move(result));
  }

 private:
  template <typename U>
  friend class Promise;

  Future(std::shared_ptr<detail::Pipe<T>> pipe) : pipe_(pipe) {}

 private:
  std::shared_ptr<detail::Pipe<T>> pipe_;
};

}  // namespace ink
