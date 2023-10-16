#pragma once

#include <chrono>

namespace ink::utils {

class Timer final {
 public:
  using Duration = std::chrono::duration<double, std::milli>;
  using TimePoint = std::chrono::_V2::system_clock::time_point;

 public:
  Timer();
  Duration GetElapsed() const noexcept;

 private:
  TimePoint start_;
};

}  // namespace ink::utils