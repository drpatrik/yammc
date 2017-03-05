#pragma once

#include "animation.h"

#include <memory>
#include <deque>

class Board {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }

  void Restart();

  std::shared_ptr<Animation> ShowHint();

  std::shared_ptr<Animation> DecreseScore();

  void BoardNotIdle();

  std::vector<std::shared_ptr<Animation>> ButtonPressed(const Position& p);

  void Render(const std::vector<std::shared_ptr<Animation>>&, double delta_timer);

  const Element& operator()(int row, int col) const { return grid_->At(row, col); }

  const AssetManager& GetAsset() const { return *asset_manager_; }

 protected:
  void UpdateScore(const std::vector<Position>& matches, int chains);
  void UpdateStatus(double delta, int x, int y);
  void RenderText(int x, int y, Font font, const std::string& text, TextColor text_color) const {
    ::RenderText(renderer_, x, y, asset_manager_->GetFont(font), text, text_color);
  }

 private:
  Position first_selected_;
  int score_ = 0;
  int displayed_score_ = 0;
  int high_score_ = 0;
  int consecutive_matches_ = 0;
  int previous_consecutive_matches_ = 0;
  int total_matches_ = 0;
  int current_threshold_step_ = kInitialThresholdStep;
  double update_score_ticks_ = 0.0;
  std::unique_ptr<Grid> grid_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::deque<std::shared_ptr<Animation>> queued_animations_;
  std::deque<std::shared_ptr<Animation>> active_animations_;
  std::shared_ptr<TimerAnimation> timer_animation_;
};
