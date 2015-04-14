#pragma once

#include <SDL2/SDL.h>
#include "grid.h"

class Animation {
 public:
  Animation() : row_(-1), col_(-1) {}
  Animation(int row, int col) : row_(row), col_(col) {}
  virtual ~Animation() noexcept = default;
  virtual bool Queue() const { return false; };
  virtual void Start(SDL_Renderer *) {}
  virtual void Update() {}
  virtual bool End() { return true; }
  virtual int row() const { return row_; }
  virtual int col() const { return col_; }

 private:
  int row_;
  int col_;
};

class NoneAnim : public Animation {
};

class UpdateMarkerAnim : public Animation {
 public:
  UpdateMarkerAnim(int row, int col) : Animation(row, col) {}
};

class SwitchAnim : public Animation {
 public:
  SwitchAnim(int row, int col, Grid<int> &grid, const Position &p1, Position &p2) : Animation(row, col), grid_(grid), p1_(p1), p2_(p2) {
  }

  virtual bool Queue() const override { return true; }

  virtual void Start(SDL_Renderer *) override {
    std::swap(grid_.At(p1_), grid_.At(p2_));
  }

  virtual void Update() override {
    ticks_++;
  }
  virtual bool End() override {
    if (ticks_ <= 10) {
      return false;
    }
    std::swap(grid_.At(p1_), grid_.At(p2_));
    return true;
  }

 private:
  Grid<int> &grid_;
  Position p1_;
  Position p2_;
  int ticks_ = 0;
};

class MatchAnim : public Animation {
 public:
  MatchAnim(int row, int col, const std::set<Position>& matches) : Animation(row, col), matches_(matches) {
  }

  virtual bool Queue() const override { return true; }

  virtual void Start(SDL_Renderer *renderer) override {
    renderer_ = renderer;
  }

  virtual void Update() override {
    ticks_++;
    if (++blink_ < 5) {
      for (auto& m:matches_) {
        SDL_Rect rc{col_to_pixel(m.second), row_to_pixel(m.first),35,35};

        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); // White
        SDL_RenderFillRect(renderer_,&rc);
      }
    }
    if (blink_ > 10) {
      blink_ = 0;
    }
  }

  virtual bool End() override {
    if (ticks_ > 30) {
      return true;
    }
    return false;
  }

 private:
  int blink_ = 0;
  int ticks_ = 0;
  std::set<Position> matches_;
  SDL_Renderer *renderer_ = nullptr;
};
