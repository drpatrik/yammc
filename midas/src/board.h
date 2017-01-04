#pragma once

#include "animation.h"

#include <memory>

class Board {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }

  void Restart();

  std::vector<std::shared_ptr<Animation>> ShowHint();

  void BoardNotIdle();

  std::vector<std::shared_ptr<Animation>> ButtonPressed(const Position& p);

  void Render(const std::vector<std::shared_ptr<Animation>>&, double delta_timer);

  const Element& operator()(int row, int col) const { return grid_->At(row, col); }

  void DecreseScore() {
    if (timer_animation_->IsReady()) {
      return;
    }
    score_ -= 20;
    score_ = std::max(score_, 0);
  }

 protected:
  enum class TextColor { White, Blue, Red, Green };
  void UpdateStatus(int x, int y);
  void RenderText(int x, int y, Font font, const std::string& text, TextColor text_color) const;

 private:
  Position first_selected_;
  int score_ = 0;
  std::unique_ptr<Grid> grid_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::vector<std::shared_ptr<Animation>> queued_animations_;
  std::vector<std::shared_ptr<Animation>> active_animations_;
  std::shared_ptr<TimerAnimation> timer_animation_;
};
