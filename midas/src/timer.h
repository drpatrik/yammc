#pragma once

#include <chrono>
#include <algorithm>

class Timer {
 public:
  using SystemClock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  explicit Timer(int value) : initial_value_(value), count_down_(value), start_(SystemClock::now()) {}

  int GetTimeInSeconds() {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(SystemClock::now() - start_).count() >= 1000) {
      start_ = SystemClock::now();
      count_down_ = std::max(--count_down_, 0);
    }
    return count_down_;
  }

  void Reset() { count_down_ = initial_value_; start_ = SystemClock::now(); }

  bool IsZero() { return GetTimeInSeconds() == 0; }

 private:
  int initial_value_;
  int count_down_;
  TimePoint start_;
};

// Adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
class DeltaTimer {
 public:
  using HighResClock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  DeltaTimer() {}

  // Returns time since last time this function was called in milliseconds
  // with nanosecond precision
  std::chrono::milliseconds GetDelta() {
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now() - previous_time_);

    previous_time_ = HighResClock::now();

    return delta;
  }

  void Reset() { previous_time_ = HighResClock::now(); }

 private:
  TimePoint previous_time_ = HighResClock::now();
};
