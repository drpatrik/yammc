#pragma once

#include "text.h"
#include "coordinates.h"

#include <vector>

class ScoreManagement final {
 public:
  ScoreManagement();

  ~ScoreManagement();

  void Reset() {
    score_ = 0;
    displayed_score_ = 0;
    new_highscore_ = (highscore_ == 0);
    update_score_ticks_ = 0.0;
    consecutive_matches_ = 0;
    previous_consecutive_matches_ = 0;
    total_matches_ = 0;
    threshold_reached_ = false;
    update_score_ticks_ = 0.0;
    current_threshold_step_ = kInitialThresholdStep;
  }

  void Update(const std::vector<Position>& matches, int chains);

  bool ThresholdReached() {
    if (threshold_reached_) {
      threshold_reached_ = false;
      return true;
    }
    return false;
  }

  bool NewHighScore() {
    if (!new_highscore_ && score_ > highscore_) {
      score_ = highscore_;
      new_highscore_ = true;
      return true;
    }
    highscore_ = std::max(highscore_, score_);
    return false;
  }

  bool ShouldPlayTimesUp() const { return score_ > 0; }

  void Decrese() { score_ = std::max(score_ - 10, 0); }

  int Get() const { return score_; }

  int GetTotalMatches() const { return total_matches_; }

  Color GetColor() const { return (displayed_score_ > score_ ) ? Color::Red : Color::White; }

  std::pair<int, int> GetDisplayedScore(double delta) {
    if (update_score_ticks_ > 0.1) {
      if (displayed_score_ > score_) {
        displayed_score_ = std::max(displayed_score_ - 2, 0);
      } else {
        displayed_score_ = score_;
      }
      if (displayed_highscore_ < highscore_) {
        displayed_highscore_ = std::min(displayed_highscore_ + 50, highscore_);
      }
      update_score_ticks_ = 0.0;
    }
    update_score_ticks_ += delta;

    return std::make_pair(displayed_score_, displayed_highscore_);
  }

  int& GetConsecutiveMatchesRef() { return consecutive_matches_; }

  int& GetPreviousConsecutiveMatchesRef() { return previous_consecutive_matches_; }

private:
  int score_ = 0;
  int displayed_score_ = 0;
  int highscore_ = 0;
  int displayed_highscore_ = 0;
  double update_score_ticks_ = 0.0;
  int consecutive_matches_ = 0;
  int previous_consecutive_matches_ = 0;
  int total_matches_ = 0;
  int current_threshold_step_ = kInitialThresholdStep;
  bool new_highscore_ = false;
  bool threshold_reached_ = false;
};

inline int GetBasicScore(size_t matches) {
  std::vector<int> scores = { 0, 0, 0, 50, 100, 150, 250, 500 };

  int score = (matches >= 7) ? 500 : scores.at(matches);

#if !defined(NDEBUG)
  if (score > 0) {
    std::cout << "GetBasicScore: " << score << std::endl;
  }
#endif

  return score;
}
