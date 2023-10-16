#include "timer.hpp"

namespace ink::utils {

using namespace std::chrono;

Timer::Timer() : start_(high_resolution_clock::now()) {}

Timer::Duration Timer::GetElapsed() const noexcept {
  return high_resolution_clock::now() - start_;
}

}  // namespace ink::utils