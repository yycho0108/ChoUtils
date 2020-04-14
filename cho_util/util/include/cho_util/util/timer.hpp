#pragma once

#include <chrono>

namespace cho_util {
namespace util {

template <typename Duration = std::chrono::duration<double, std::milli>>
class Timer {
 public:
  using Clock = std::chrono::high_resolution_clock;
  using Count = typename Duration::rep;

  inline Timer(const bool start = false) {
    if (start) {
      this->Start();
    }
  }

  inline void Start() { start_time_ = Clock::now(); }

  inline void Stop() { stop_time_ = Clock::now(); }

  inline Count GetElapsedTimeSinceStart() const {
    return std::chrono::duration_cast<Duration>(Clock::now() - start_time_)
        .count();
  }

  inline Count GetElapsedTime() const {
    return std::chrono::duration_cast<Duration>(stop_time_ - start_time_)
        .count();
  }

  inline Count StopAndGetElapsedTime() {
    Stop();
    return GetElapsedTime();
  }

 private:
  std::chrono::time_point<Clock> start_time_;
  std::chrono::time_point<Clock> stop_time_;
};

using MTimer = Timer<std::chrono::duration<double, std::milli>>;
using UTimer = Timer<std::chrono::duration<double, std::micro>>;

}  // namespace util
}  // namespace cho_util
