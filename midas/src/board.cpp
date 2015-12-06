#include <iostream>
#include <algorithm>

#include "board.h"
#include "coordinates.h"

const int kWidth = 755;
const int kHeight = 600;
const std::pair<int, int> kEmptyMarker{-1, -1};

Board::Board() {
  window_ = SDL_CreateWindow("Yet Another Midas Clone", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, 0);
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
  timer_ = kGameTime;
  board_busy_ = false;
  first_marker_ = kEmptyMarker;
  grid_ = std::make_unique<Grid<int>>();
  active_animations_.clear();
  queued_animations_.clear();
}

inline bool valid_second_choice(const std::pair<int, int>& old_pos,
                                const std::pair<int, int>& new_pos) {
  int c = (std::make_pair(old_pos.first + 1, old_pos.second) == new_pos);

  c += (std::make_pair(old_pos.first - 1, old_pos.second) == new_pos);
  c += (std::make_pair(old_pos.first, old_pos.second + 1) == new_pos);
  c += (std::make_pair(old_pos.first, old_pos.second - 1) == new_pos);

  return (c > 0);
}

std::vector<std::shared_ptr<Animation>> Board::GetInteraction(int row, int col) {
  std::vector<std::shared_ptr<Animation>> animations;

  if (board_busy_ || !timer_() || row == -1 || col == -1) {
    animations.push_back(std::make_shared<NoneAnim>());

    return animations;
  }
  auto new_pos = std::make_pair(row, col);

  if (kEmptyMarker == first_marker_) {
    first_marker_ = {row, col};
  } else {
    if (valid_second_choice(first_marker_, new_pos)) {
      auto matches = grid_->Switch(first_marker_, new_pos);

      animations.push_back(std::make_shared<SwitchAnim>(row, col, *grid_.get(), first_marker_, new_pos, !matches.empty(), asset_manager_));
      if (!matches.empty()) {
        animations.push_back(std::make_shared<MatchAnim>(row, col, matches));
      }
    }
    first_marker_ = {-1, -1};
  }
  if (animations.empty()) {
    animations.push_back(std::make_shared<UpdateMarkerAnim>(row, col));
  }

  return animations;
}

std::shared_ptr<Animation> Board::Render(std::vector<std::shared_ptr<Animation>>& animations) {
  SDL_Rect rc{0, 0, kWidth, kHeight};

  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, asset_manager_->GetBackgroundTexture(), nullptr, &rc);

  int row = animations[0]->row();
  int col = animations[0]->col();

  if (timer_()) {
    grid_->for_each([this](int row, int col, int id) {
      SDL_Rect rc{col_to_pixel(col), row_to_pixel(row), 35, 35};

      if (id >= 0) {
        SDL_RenderCopy(renderer_, asset_manager_->GetTexture(id), nullptr, &rc);
      }
    });
    if (kEmptyMarker != first_marker_) {
      SDL_Rect rc{col_to_pixel(animations[0]->col()),
                  row_to_pixel(animations[0]->row()), 35, 35};

      SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);  // White
      SDL_RenderDrawRect(renderer_, &rc);
    }
    for (auto a:animations) {
      if (a->Queue()) {
        queued_animations_.push_back(a);
      }
    }
    if (active_animations_.empty() && !queued_animations_.empty()) {
      auto animation = queued_animations_.front();
      queued_animations_.erase(std::begin(queued_animations_));

      board_busy_ = true;
      animation->Start(renderer_);
      active_animations_.push_back(animation);
    }
    auto it = std::begin(active_animations_);

    while (it != std::end(active_animations_)) {
      (*it)->Update();
      if ((*it)->End()) {
        it = active_animations_.erase(it);
      } else {
        ++it;
      }
    }
    if (active_animations_.empty() && queued_animations_.empty()) {
      int new_score;
      std::vector<Position> new_objects;

      std::tie(board_busy_, new_score) = grid_->Collaps(new_objects);
      score_ += new_score;

      for (auto no:new_objects) {
        auto animation = std::make_shared<MoveDownAnim>(row, col, *grid_.get(), no, asset_manager_);

        animation->Start(renderer_);
        active_animations_.push_back(animation);
      }
    }
  } else {
    RenderText(400, 233, Font::Bold, "G A M E  O V E R");
  }
  UpdateStatus(10, 10);
  SDL_RenderPresent(renderer_);

  return std::make_shared<UpdateMarkerAnim>(row, col);
}

void Board::RenderText(int x, int y, Font font, const std::string& text) {
  const SDL_Color color{255, 255, 255, 255};

  SDL_Surface* surface = TTF_RenderText_Blended(asset_manager_->GetFont(font),
                                                text.c_str(), color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

  int iW, iH;

  SDL_QueryTexture(texture, NULL, NULL, &iW, &iH);

  SDL_Rect rc{x, y, iW, iH};

  SDL_RenderCopy(renderer_, texture, nullptr, &rc);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Board::UpdateStatus(int x, int y) {
  RenderText(x, y + 10, Font::Bold, "Score:");
  RenderText(x, y + 40, Font::Normal, std::to_string(score_));

  RenderText(x, y + 75, Font::Bold, "Timer:");
  RenderText(x, y + 105, Font::Normal, std::to_string(timer_()));
}
