#include "board.h"
#include "score.h"

#include <sstream>
#include <iomanip>

namespace {

const SDL_Rect kClipRect { 0, kBoardStartY, kWidth, kHeight }; // We only care about Y position
const Position kNothingSelected { -1, -1 };

bool IsSwapValid(const Position& old_pos, const Position& new_pos) {
  int c = (new_pos == std::make_pair(old_pos.row() + 1, old_pos.col()));

  c += (new_pos == std::make_pair(old_pos.row() - 1, old_pos.col()));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() + 1));
  c += (new_pos == std::make_pair(old_pos.row(), old_pos.col() - 1));

  return (c > 0);
}

bool RunAnimation(std::deque<std::shared_ptr<Animation>>& animations, double delta_time) {
  for (auto it = std::begin(animations); it != std::end(animations);) {
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
  for (auto it = std::begin(animations); it != std::end(animations);) {
    if ((*it)->Idle()) {
      it = animations.erase(it);
    } else {
      ++it;
    }
  }
}

bool CanUpdateBoard(const std::deque<std::shared_ptr<Animation>>& animations) {
  auto c = std::count_if(std::begin(animations), std::end(animations), [] (const auto& a) { return a->LockBoard(); });

  return animations.empty() || (c == 0);
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
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);

  asset_manager_ = std::make_shared<AssetManager>(renderer_);

  Restart();
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Restart(bool music_on) {
  score_.Reset();
  game_over_ = false;
  active_animations_.clear();
  queued_animations_.clear();
  first_selected_ = kNothingSelected;
  grid_ = std::make_unique<Grid>(kRows, kCols, asset_manager_.get());
  timer_animation_ = std::make_shared<TimerAnimation>(renderer_, *grid_.get(), asset_manager_);
  asset_manager_->GetAudio().StopSound();
  if (music_on) {
    asset_manager_->GetAudio().PlayMusic();
  }
}

std::shared_ptr<Animation> Board::ShowHint() {
  if (timer_animation_->IsReady()) {
    return nullptr;
  }
  if (auto [matches_found, match_pos] = grid_->FindPotentialMatches(); matches_found) {
    return std::make_shared<HintAnimation>(renderer_, *grid_, match_pos.first, match_pos.second, asset_manager_);
  }
  return nullptr;
}

void Board::DecreseScore() {
    if (timer_animation_->IsReady()) {
      return;
    }
    if (score_.ShouldPlayTimesUp()) {
      asset_manager_->GetAudio().PlaySound(TimesUp, 500);
    }
    score_.Decrese();
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
      auto [matches, chains] = grid_->GetMatchesFromSwap(first_selected_, selected);

      animations.emplace_back(std::make_shared<SwapAnimation>(renderer_, *grid_, first_selected_, selected, !matches.empty(), asset_manager_));

      if (!matches.empty()) {
        score_.Update(matches, chains);
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
    if (!game_over_) {
      GetAsset().GetAudio().StopMusic();
      RemoveIdleAnimations(active_animations_);
      active_animations_.clear();
      game_over_ = true;
    }
    if (active_animations_.size() == 0) {
      ActivateAnimation<ExplosionAnimation>(renderer_, *grid_, asset_manager_);
    }
    RenderText(400, 233, Font::Bold, "G A M E  O V E R", Color::Red);
    RunAnimation(active_animations_, delta_time);
  } else {
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
    if (score_.ThresholdReached()) {
      // This animation does not lock the board so we can add it directly to the
      // active animation queue
      ActivateAnimation<ThresholdReachedAnimation>(renderer_, *grid_, asset_manager_, score_.GetTotalMatches());
    }
    RunAnimation(active_animations_, delta_time);

    if (CanUpdateBoard(active_animations_) && CanUpdateBoard(queued_animations_)) {
      auto [moved_objects, matches, chains] = grid_->Collaps(score_.GetConsecutiveMatchesRef(),
                                                                score_.GetPreviousConsecutiveMatchesRef());

      score_.Update(matches, chains);

      if (!matches.empty()) {
        ActivateAnimation<MatchAnimation>(renderer_, *grid_, matches, chains, asset_manager_);
      }
      for (const auto& obj:moved_objects) {
        ActivateAnimation<MoveDownAnimation>(renderer_, *grid_, obj, asset_manager_);
      }
    }
    timer_animation_->Update(delta_time);
    SDL_RenderSetClipRect(renderer_, nullptr);
  }
  UpdateStatus(delta_time, 10, 1);
  SDL_RenderPresent(renderer_);
}

void Board::UpdateStatus(double delta, int x, int y) {
  if (score_.NewHighScore()) {
    asset_manager_->GetAudio().PlaySound(HighScore);
  }
  auto [score, highscore] = score_.GetDisplayedScore(delta);

  RenderText(x, y, Font::Normal, "Score:", Color::White);
  RenderText(x + 74, y, Font::Normal, std::to_string(score), score_.GetColor());
  RenderText(x + 520, y, Font::Normal, "High Score:", Color::White);
  RenderText(x + 650, y, Font::Normal, std::to_string(highscore), Color::White);
  RenderText(x + 72, y + 430, Font::Bold, FormatTime(timer_animation_->GetTimeLeft()), Color::Blue);
}
