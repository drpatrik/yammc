#pragma once

class Timer {
 public:
  explicit Timer(int timer) : timer_(timer), start_(SDL_GetTicks()) {}

  int operator()() {
    if (SDL_GetTicks() - start_ >= 1000) {
      start_ = SDL_GetTicks();
      timer_ = std::max(--timer_, 0);
    }
    return timer_;
  }

 private:
  int timer_;
  int start_;
};
