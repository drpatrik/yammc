#include "board.h"
#include "coordinates.h"

#include <algorithm>

namespace {

const int kWidth = 755;
const int kHeight = 600;
const Position kNothingSelected{ -1, -1 };

bool IsValidMove(const Position& old_pos, const Position& new_pos) {
  int c = (new_pos == std::make_pair(old_pos.row() + 1, old_pos.col()));

  c += (new_pos == std::make_pair(old_pos.row() - 1, old_pos.col()));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() + 1));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() - 1));

  return (c > 0);
}

}

Board::Board() {
  window_ = SDL_CreateWindow("Yet Another Midas Clone", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_OPENGL);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError();
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, 0);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError();
    exit(-1);
  }
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);

  asset_manager_ = std::make_shared<AssetManager>(renderer_);

  Restart();
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Restart() {
  score_ = 0;
  timer_.Reset();

  first_selected_ = kNothingSelected;
  grid_ = std::make_unique<Grid>(kRows, kCols, asset_manager_.get());
  active_animations_.clear();
  queued_animations_.clear();
}

std::vector<std::shared_ptr<Animation>> Board::ButtonPressed(const Position& p) {
  std::vector<std::shared_ptr<Animation>> animations;

  if (timer_.IsZero() || !p.IsValid() || grid_->IsFilling()) {
    return animations;
  }
  auto selected = p;

  if (kNothingSelected == first_selected_) {
    grid_->At(selected).Select();
    first_selected_ = selected;
  } else {
    if (IsValidMove(first_selected_, selected)) {
      auto matches = grid_->Switch(first_selected_, selected);

      animations.push_back(std::make_shared<SwitchAnimation>(renderer_, *grid_, first_selected_, selected, !matches.empty(), asset_manager_));

      if (!matches.empty()) {
        animations.push_back(std::make_shared<MatchAnimation>(renderer_, *grid_, matches, asset_manager_));
      }
      score_ += matches.size();
    } else {
      grid_->At(first_selected_).Unselect();
    }
    first_selected_ = kNothingSelected;
  }
  return animations;
}

void Board::Render(const std::vector<std::shared_ptr<Animation>>& animations) {
  SDL_Rect rc{ 0, 0, kWidth, kHeight};
  SDL_Rect clip_rc{ kBoardStartX, kBoardStartY, kWidth, kHeight };

  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, asset_manager_->GetBackgroundTexture(), nullptr, &rc);

  if (timer_.IsZero()) {
    RenderText(400, 233, Font::Bold, "G A M E  O V E R");
    UpdateStatus(10, 10);
    SDL_RenderPresent(renderer_);
    return;
  }
  grid_->Render(renderer_);
  SDL_RenderSetClipRect(renderer_, &clip_rc);

  for (const auto& a:animations) {
    queued_animations_.push_back(a);
  }
  if (active_animations_.empty() && !queued_animations_.empty()) {
    auto animation = queued_animations_.front();
    queued_animations_.erase(std::begin(queued_animations_));
    animation->Start();
    active_animations_.push_back(animation);
  }
  auto it = std::begin(active_animations_);

  while (it != std::end(active_animations_)) {
    (*it)->Update();
    if ((*it)->IsDone()) {
      it = active_animations_.erase(it);
    } else {
      ++it;
    }
  }
  if (active_animations_.empty() && queued_animations_.empty()) {
    std::vector<Position> moved_objects;
    std::set<Position> matches;

    std::tie(moved_objects, matches) = grid_->Collaps();
    score_ += matches.size();

    for (const auto& obj:moved_objects) {
      auto animation = std::make_shared<MoveDownAnimation>(renderer_, *grid_, obj, asset_manager_);

      animation->Start();
      active_animations_.push_back(animation);
    }
    if (!matches.empty()) {
      auto animation = std::make_shared<MatchAnimation>(renderer_, *grid_, matches, asset_manager_);
      animation->Start();
      active_animations_.push_back(animation);
    }
  }
  SDL_RenderSetClipRect(renderer_, &rc);
  UpdateStatus(10, 10);
  SDL_RenderPresent(renderer_);
}

void Board::RenderText(int x, int y, Font font, const std::string& text) {
  const SDL_Color color { 255, 255, 255, 255 };

  SDL_Surface* surface = TTF_RenderText_Blended(asset_manager_->GetFont(font), text.c_str(), color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

  int width, height;

  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  SDL_Rect rc { x, y, width, height };

  SDL_RenderCopy(renderer_, texture, nullptr, &rc);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Board::UpdateStatus(int x, int y) {
  RenderText(x, y + 10, Font::Bold, "Score:");
  RenderText(x, y + 40, Font::Normal, std::to_string(score_));

  RenderText(x, y + 75, Font::Bold, "Timer:");
  RenderText(x, y + 105, Font::Normal, std::to_string(timer_.GetTimeInSeconds()));
}
