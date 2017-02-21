#include "board.h"
#include "score.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <cassert>

namespace {

const SDL_Rect kClipRect { kBoardStartX, kBoardStartY, kWidth, kHeight };
const Position kNothingSelected { -1, -1 };
const std::string kFilename("midas.shs");

void SaveHighscore(int high_score) {
  std::ofstream fs(kFilename);

  if (!fs) {
    std::cout << "Failed to save highscore to disk" << std::endl;
    return;
  }
  fs << high_score << std::endl;
}

int ReadHighscore() {
  std::ifstream fs(kFilename);

  if (!fs) {
    return 0.0;
  }
  int high_score;

  fs >> high_score;

  return high_score;
}

bool IsSwapValid(const Position& old_pos, const Position& new_pos) {
  int c = (new_pos == std::make_pair(old_pos.row() + 1, old_pos.col()));

  c += (new_pos == std::make_pair(old_pos.row() - 1, old_pos.col()));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() + 1));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() - 1));

  return (c > 0);
}

bool RunAnimation(std::deque<std::shared_ptr<Animation>>& animations, double delta_time) {
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

void RemoveIdleAnimations(std::deque<std::shared_ptr<Animation>>& animations) {
  auto it = std::begin(animations);

  while (it != std::end(animations)) {
    if ((*it)->Idle())
      it = animations.erase(it);
    break;
  }
}

bool CanUpdateBoard(const std::deque<std::shared_ptr<Animation>>& animations) {
  auto c = std::count_if(std::begin(animations), std::end(animations), [] (const auto& a) { return a->LockBoard(); });

  return animations.empty() || (c == 0);
}

int CalculateScore(size_t matches, int& total_matches, int& current_threshold_step, int consecutive_matches, int& previous_consecutive_matches) {
  int score = 0;

  total_matches += matches;
  score += GetBasicScore(matches);
  score += GetScoreForTotalMatches(total_matches, current_threshold_step);
  score += GetScoreForConsecutiveMatches(consecutive_matches, previous_consecutive_matches);

  return score;
}

std::string FormatTime(size_t seconds) {
  std::stringstream ss;

  int minutes = static_cast<int>(seconds / 60.0);

  ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds % 60;

  return ss.str();
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

  high_score_ = ReadHighscore();

  Restart();
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SaveHighscore(high_score_);
}

void Board::Restart() {
  score_ = 0;
  consecutive_matches_ = 0;
  previous_consecutive_matches_ = 0;
  total_matches_ = 0;
  current_threshold_step_ = kInitialThresholdStep;
  active_animations_.clear();
  queued_animations_.clear();
  first_selected_ = kNothingSelected;
  grid_ = std::make_unique<Grid>(kRows, kCols, asset_manager_.get());
  timer_animation_ = std::make_shared<TimerAnimation>(renderer_, *grid_.get(), asset_manager_);
}

std::vector<std::shared_ptr<Animation>> Board::ShowHint() {
  std::vector<std::shared_ptr<Animation>> animations;

  if (timer_animation_->IsReady()) {
    return animations;
  }
  bool matches_found;
  std::pair<Position, Position> match_pos;

  std::tie(matches_found, match_pos) = grid_->FindPotentialMatches();

  if (matches_found) {
    animations.emplace_back(std::make_shared<HintAnimation>(renderer_, *grid_, match_pos.first, match_pos.second, asset_manager_));
  } else {
    assert(false);
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
      std::vector<Position> matches;
      int chains;
      std::tie(matches, chains) = grid_->GetMatchesFromSwap(first_selected_, selected);

      animations.emplace_back(std::make_shared<SwapAnimation>(renderer_, *grid_, first_selected_, selected, !matches.empty(), asset_manager_));

      if (!matches.empty()) {
        UpdateScore(matches, chains);
        animations.emplace_back(std::make_shared<MatchAnimation>(renderer_, *grid_, matches, chains, asset_manager_));
      }
    } else {
      grid_->At(first_selected_).Unselect();
    }
    first_selected_ = kNothingSelected;
  }
  return animations;
}

void Board::Render(const std::vector<std::shared_ptr<Animation>>& animations, double delta_time) {
  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, asset_manager_->GetBackgroundTexture(), nullptr, nullptr);

  if (timer_animation_->IsReady()) {
    RemoveIdleAnimations(active_animations_);
    if (active_animations_.size() == 0) {
      active_animations_.emplace_back(std::make_shared<ExplosionAnimation>(renderer_, *grid_, asset_manager_));
    }
    RenderText(400, 233, Font::Bold, "G A M E  O V E R", TextColor::Red);
    UpdateStatus(10, 2);
    RunAnimation(active_animations_, delta_time);
    SDL_RenderPresent(renderer_);
    return;
  }
  timer_animation_->Update(delta_time);

  grid_->Render(renderer_);
  SDL_RenderSetClipRect(renderer_, &kClipRect);

  for (const auto& a:animations) {
    queued_animations_.emplace_back(a);
  }
  if (CanUpdateBoard(active_animations_) && !queued_animations_.empty()) {
    auto animation = queued_animations_.front();
    queued_animations_.pop_front();
    animation->Start();
    active_animations_.push_front(animation);
  }
  RunAnimation(active_animations_, delta_time);

  if (CanUpdateBoard(active_animations_) && CanUpdateBoard(queued_animations_)) {
    std::vector<Position> moved_objects;
    std::vector<Position> matches;
    int chains;

    std::tie(moved_objects, matches, chains) = grid_->Collaps(consecutive_matches_, previous_consecutive_matches_);

    UpdateScore(matches, chains);

    if (!matches.empty()) {
      auto animation = std::make_shared<MatchAnimation>(renderer_, *grid_, matches, chains, asset_manager_);
      animation->Start();
      // Ensure that match animations are first in the queue
      // for Z-order reasons
      active_animations_.push_front(animation);
    }
    for (const auto& obj:moved_objects) {
      auto animation = std::make_shared<MoveDownAnimation>(renderer_, *grid_, obj, asset_manager_);

      animation->Start();
      active_animations_.push_front(animation);
    }
  }
  SDL_RenderSetClipRect(renderer_, nullptr);
  UpdateStatus(10, 1);
  SDL_RenderPresent(renderer_);
}

void Board::UpdateScore(const std::vector<Position>& matches, int chains) {
  high_score_ = std::max(high_score_, score_);
  consecutive_matches_ += chains;

  size_t unique_matches = 0;

  if (matches.size() > 0) {
    unique_matches = std::set<Position>(matches.begin(), matches.end()).size();
  }
  score_ += CalculateScore(unique_matches, total_matches_, current_threshold_step_, consecutive_matches_, previous_consecutive_matches_);
}

void Board::UpdateStatus(int x, int y) const {
  RenderText(x, y, Font::Normal, "Score:", TextColor::White);
  RenderText(x + 74, y, Font::Normal, std::to_string(score_), TextColor::White);

  RenderText(x + 520, y, Font::Normal, "High Score:", TextColor::White);
  RenderText(x + 650, y, Font::Normal, std::to_string(high_score_), TextColor::White);

  RenderText(x + 72, y + 430, Font::Bold, FormatTime(timer_animation_->GetTimeLeft()), TextColor::Blue);
}
