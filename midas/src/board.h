#pragma once

#include "animation.h"
#include "timer.h"

#include <vector>
#include <memory>

class Board {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }

  void Restart();

  std::vector<std::shared_ptr<Animation>> ButtonPressed(int row, int col);

  void Render(const std::vector<std::shared_ptr<Animation>>&);

  auto operator()(int row, int col) { return grid_->At(row, col); }

 protected:
  void RenderText(int x, int y, Font font, const std::string& text);
  void UpdateStatus(int x, int y);

 private:
  Position first_selected_;
  int score_ = 0;
  Timer timer_ = Timer(kGameTime);
  std::unique_ptr<Grid> grid_;
  std::shared_ptr<AssetManager> asset_manager_;

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::vector<std::shared_ptr<Animation>> queued_animations_;
  std::vector<std::shared_ptr<Animation>> active_animations_;
};
