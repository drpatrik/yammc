#pragma once

#include "animation.h"

#include <memory>
#include <deque>

class Board final {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }

  void Restart(bool music_on = true);

  bool IsGameOver() const { return game_over_; }

  std::shared_ptr<Animation> ShowHint();

  void DecreseScore();

  void BoardNotIdle();

  std::vector<std::shared_ptr<Animation>> ButtonPressed(const Position& p);

  void Render(const std::vector<std::shared_ptr<Animation>>&, double delta_timer);

  const Element& operator()(int row, int col) const { return grid_->At(row, col); }

  const AssetManager& GetAsset() const { return *asset_manager_; }

 protected:
  template<class T, class ...Args>
  void ActivateAnimation(Args&&... args) {
    auto animation = std::make_shared<T>(std::forward<Args>(args)...);

    animation->Start();
    active_animations_.push_front(animation);
  }

  void UpdateStatus(double delta, int x, int y);

  void RenderText(int x, int y, Font font, const std::string& text, Color text_color) const {
    ::RenderText(renderer_, x, y, asset_manager_->GetFont(font), text, text_color);
  }

 private:
  ScoreManagement score_;
  bool game_over_ = false;
  Position first_selected_;
  std::unique_ptr<Grid> grid_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::deque<std::shared_ptr<Animation>> queued_animations_;
  std::deque<std::shared_ptr<Animation>> active_animations_;
  std::shared_ptr<TimerAnimation> timer_animation_;
  bool set_window_size_ = true;
};
