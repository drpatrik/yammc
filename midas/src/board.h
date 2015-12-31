#pragma once

#include <vector>
#include <memory>

#include "animation.h"
#include "element.h"
#include "timer.h"

class Board {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }

  void Restart();

  std::vector<std::shared_ptr<Animation>> GetInteraction(int row, int col);

  std::shared_ptr<Animation> Render(std::vector<std::shared_ptr<Animation>>&);

  auto operator()(int row, int col) { return grid_->At(row, col); }

 protected:
  void RenderText(int x, int y, Font font, const std::string& text);
  void UpdateStatus(int x, int y);

 private:
  std::pair<int, int> first_marker_ = {-1, -1};
  bool board_busy_ = false;
  int score_ = 0;
  Timer timer_ = kGameTime;
  std::unique_ptr<Grid> grid_;
  std::shared_ptr<AssetManager> asset_manager_;

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::vector<std::shared_ptr<Animation>> queued_animations_;
  std::vector<std::shared_ptr<Animation>> active_animations_;
};
