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

  std::vector<std::shared_ptr<Animation>> ButtonPressed(const Position& p);

  void Render(const std::vector<std::shared_ptr<Animation>>&);

  const Element& operator()(int row, int col) const { return grid_->At(row, col); }

 protected:
  void UpdateStatus(int x, int y);
  void RenderText(int x, int y, Font font, const std::string& text) const;

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
