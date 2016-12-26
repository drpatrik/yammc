#pragma once

#include <chrono>

// Adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
class DeltaTimer {
 public:
  using HighResClock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  DeltaTimer() : previous_time_(HighResClock::now()) {}

  // Returns time since last time this function was called
  // in seconds with nanosecond precision
  double GetDelta() {
    // 1. Get current time as a std::chrono::time_point
    auto current_time = HighResClock::now();

    // 2. Get the time difference as seconds
    // ...represented as a double
    std::chrono::duration<double> delta {current_time - previous_time_};

    // 3. Reset the timePrev to the current point in time
    previous_time_ = HighResClock::now();

    // 4. Returns the number of ticks in delta
    return delta.count();
  }

  void Reset() { previous_time_ = HighResClock::now(); }

 private:
  TimePoint previous_time_;
};
