#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <thread>
#include <memory>
#include <string>
#include <variant>

#include <core/include/futures/promise.hpp>

namespace ink::test {

using namespace std::chrono_literals;

TEST(Promise, TrivialSetValue) {
  Promise<int> promise;
  auto future = promise.MakeFuture();
  promise.SetValue(5);
  EXPECT_EQ(future.GetValue(), 5);
}

TEST(Promise, TrivialSetValueException) {
  Promise<int> promise;
  auto future = promise.MakeFuture();
  try {
      throw std::runtime_error("SomethingWrong");
    } catch (const std::runtime_error& ex) {
      promise.SetException(std::make_exception_ptr(ex));
  }
  try {
    EXPECT_THROW(future.GetValue(), std::runtime_error);
  } catch (const std::runtime_error& ex) {
    EXPECT_EQ(ex.what(), "SomethingWrong");
  }
}

TEST(Promise, ValueFromThread) {
  Promise<std::string> promise;
  auto future = promise.MakeFuture();

  std::thread producer([promise = std::move(promise)]() mutable {
    std::this_thread::sleep_for(50ms);
    promise.SetValue("Hello");
  });

  EXPECT_EQ(future.GetValue(), "Hello");
  producer.join();
}

struct MoveOnly final {
  MoveOnly(std::size_t state) : state(state) {};

  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&&) noexcept = default;
  MoveOnly& operator=(MoveOnly&&) noexcept = default;

  std::size_t state;
};

TEST(Promise, MoveOnly) {
  Promise<MoveOnly> promise;
  auto future = promise.MakeFuture();

  std::thread producer([promise = std::move(promise)]() mutable {
    std::this_thread::sleep_for(50ms);
    promise.SetValue(MoveOnly{5});
  });

  EXPECT_EQ(future.GetValue().state, 5);
  producer.join();
}

TEST(Promise, ExceptionFromThread) {
  Promise<double> promise;
  auto future = promise.MakeFuture();

  std::thread producer([promise = std::move(promise)]() mutable {
    std::this_thread::sleep_for(50ms);
    try {
      throw std::runtime_error("SomethingWrong");
    } catch (const std::runtime_error& ex) {
      promise.SetException(std::make_exception_ptr(ex));
    }
  });

  try {
    EXPECT_THROW(future.GetValue(), std::runtime_error);
  } catch (const std::runtime_error& ex) {
    EXPECT_EQ(ex.what(), "SomethingWrong");
  }
  
  producer.join();
}

TEST(Promise, Monostate) {
  Promise<std::monostate> promise;
  auto future = promise.MakeFuture();
  std::atomic<bool> is_reached_ = false;

  std::thread producer([promise = std::move(promise), &is_reached_]() mutable {
    std::this_thread::sleep_for(50ms);
    promise.SetValue({});
    is_reached_.store(true);
  });

  future.GetValue();
  EXPECT_TRUE(is_reached_.load());
  
  producer.join();
}

struct KekException {};

TEST(Promise, ExceptionAsValue) {
  Promise<std::exception_ptr> promise;
  auto future = promise.MakeFuture();

  std::thread producer([promise = std::move(promise)]() mutable {
    std::this_thread::sleep_for(50ms);
    try {
      throw KekException{};
    } catch (...) {
      promise.SetValue(std::current_exception());
    }
  });

  std::exception_ptr eptr;
  EXPECT_NO_THROW(eptr = future.GetValue());
  EXPECT_THROW(std::rethrow_exception(eptr), KekException);

  producer.join();
}

}  // namespace ink::test
