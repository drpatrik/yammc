#include "board.h"

namespace {

const int kWidth = 755;
const int kHeight = 600;
const Position kNothingSelected{ -1, -1 };

bool IsSwapValid(const Position& old_pos, const Position& new_pos) {
  int c = (new_pos == std::make_pair(old_pos.row() + 1, old_pos.col()));

  c += (new_pos == std::make_pair(old_pos.row() - 1, old_pos.col()));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() + 1));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() - 1));

  return (c > 0);
}

bool RunAnimation(std::vector<std::shared_ptr<Animation>>& animations, double delta_time) {
  auto it = std::begin(animations);

  while (it != std::end(animations)) {
    (*it)->Update(delta_time);
    if ((*it)->IsReady()) {
      it = animations.erase(it);
    } else {
      ++it;
    }
  }

  return (animations.size() == 0);
}

void RemoveIdleAnimations(std::vector<std::shared_ptr<Animation>>& animations) {
  auto it = std::begin(animations);

  while (it != std::end(animations)) {
    if ((*it)->Idle())
      it = animations.erase(it);
    break;
  }
}

bool CanUpdateBoard(const std::vector<std::shared_ptr<Animation>>& animations) {
  auto c = std::count_if(std::begin(animations), std::end(animations), [] (const auto& a) { return a->LockBoard(); });

  return animations.empty() || (c == 0);
}

int GetScoreForTotalMatches(int total_matches, int& current_threshold_step) {
  int score = 0;

  if (total_matches >= (current_threshold_step * kThresholdMultiplier)) {
    score = (500 + ((current_threshold_step - kInitialThresholdStep) * 250));
    current_threshold_step++;
  }

  return score;
}

int GetBasicScore(size_t matches, int& total_matches, int& current_threshold_step) {
  std::vector<int> scores = { 0, 0, 0, 50, 100, 150, 250, 500 };

  total_matches += matches;
  int score = (matches >= 7) ? 500 : scores.at(matches);

  score += GetScoreForTotalMatches(total_matches, current_threshold_step);

  return score;
}

int GetScoreForConsecutiveMatches(size_t consecutive_matches) {
  std::vector<int> scores = { 0, 0, 50, 100, 150, 250, 350, 500, 750 };

  return (consecutive_matches >=9) ? 1000 : scores.at(consecutive_matches);
}

}

Board::Board() {
  window_ = SDL_CreateWindow("Yet Another Midas Clone", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_OPENGL);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
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
  consecutive_matches_ = 0;
  total_matches_ = 0;
  current_threshold_step_ = kInitialThresholdStep;
  active_animations_.clear();
  queued_animations_.clear();
  first_selected_ = kNothingSelected;
  grid_ = std::make_unique<Grid>(kRows, kCols, asset_manager_.get());
  timer_animation_ = std::make_shared<TimerAnimation>(renderer_, *grid_.get(), asset_manager_);
}

std::vector<std::shared_ptr<Animation>> Board::ShowHint() {
  bool matches_found;
  std::pair<Position, Position> match_pos;
  std::vector<std::shared_ptr<Animation>> animations;

  std::tie(matches_found, match_pos) = grid_->FindPotentialMatches();

  if (matches_found) {
    animations.push_back(std::make_shared<HintAnimation>(renderer_, *grid_, match_pos.first, match_pos.second, asset_manager_));
  }

  return animations;
}

void Board::BoardNotIdle() {
  RemoveIdleAnimations(active_animations_);
  RemoveIdleAnimations(queued_animations_);
}

std::vector<std::shared_ptr<Animation>> Board::ButtonPressed(const Position& p) {
  std::vector<std::shared_ptr<Animation>> animations;

  if (timer_animation_->IsReady() || !p.IsValid() || grid_->IsFilling()) {
    return animations;
  }
  auto selected = p;

  if (kNothingSelected == first_selected_) {
    grid_->At(selected).Select();
    first_selected_ = selected;
  } else {
    if (IsSwapValid(first_selected_, selected)) {
      auto matches = grid_->GetMatchesFromSwap(first_selected_, selected);

      animations.push_back(std::make_shared<SwapAnimation>(renderer_, *grid_, first_selected_, selected, !matches.empty(), asset_manager_));

      if (!matches.empty()) {
        animations.push_back(std::make_shared<MatchAnimation>(renderer_, *grid_, matches, asset_manager_));
        score_ += GetBasicScore(matches.size(), total_matches_, current_threshold_step_);
        score_ += GetScoreForConsecutiveMatches(consecutive_matches_);
        consecutive_matches_++;
      }
    } else {
      grid_->At(first_selected_).Unselect();
    }
    first_selected_ = kNothingSelected;
  }
  return animations;
}

void Board::Render(const std::vector<std::shared_ptr<Animation>>& animations, double delta_time) {
  SDL_Rect rc{ 0, 0, kWidth, kHeight};
  SDL_Rect clip_rc{ kBoardStartX, kBoardStartY, kWidth, kHeight };

  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, asset_manager_->GetBackgroundTexture(), nullptr, &rc);

  if (timer_animation_->IsReady()) {
    RemoveIdleAnimations(active_animations_);
    if (active_animations_.size() == 0) {
      active_animations_.push_back(std::make_shared<ExplosionAnimation>(renderer_, *grid_, asset_manager_));
    }
    RenderText(400, 233, Font::Bold, "G A M E  O V E R", TextColor::Red);
    UpdateStatus(10, 10);
    RunAnimation(active_animations_, delta_time);
    SDL_RenderPresent(renderer_);
    return;
  }
  timer_animation_->Update(delta_time);

  grid_->Render(renderer_);
  SDL_RenderSetClipRect(renderer_, &clip_rc);

  for (const auto& a:animations) {
    queued_animations_.push_back(a);
  }
  if (CanUpdateBoard(active_animations_) && !queued_animations_.empty()) {
    auto animation = queued_animations_.front();
    queued_animations_.erase(std::begin(queued_animations_));
    animation->Start();
    active_animations_.push_back(animation);
  }

  RunAnimation(active_animations_, delta_time);

  if (CanUpdateBoard(active_animations_) && CanUpdateBoard(queued_animations_)) {
    std::vector<Position> moved_objects;
    std::set<Position> matches;

    std::tie(moved_objects, matches) = grid_->Collaps();
    score_ += GetBasicScore(matches.size(), total_matches_, current_threshold_step_);

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

void Board::UpdateStatus(int x, int y) {
  high_score_ = std::max(high_score_, score_);
  RenderText(x, y, Font::Bold, "Score:", TextColor::White);
  RenderText(x, y + 30, Font::Normal, std::to_string(score_), TextColor::White);

  RenderText(x, y + 530, Font::Bold, "High Score:", TextColor::Black);
  RenderText(x, y + 560, Font::Normal, std::to_string(high_score_), TextColor::Black);

  RenderText(x + 92, y + 430, Font::Bold, std::to_string(timer_animation_->GetTimeLeft()), TextColor::Blue);
}
