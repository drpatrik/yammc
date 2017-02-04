#pragma once

#include "constants.h"

#include <vector>

inline int GetScoreForTotalMatches(int total_matches, int& current_threshold_step) {
  int score = 0;

  if (total_matches >= (current_threshold_step * kThresholdMultiplier)) {
    score = (500 + ((current_threshold_step - kInitialThresholdStep) * 250));
    current_threshold_step++;

#if !defined(NDEBUG)
    if (score > 0) {
      std::cout << "GetScoreForTotalMatches: " << score << "N Jewels: " << total_matches << std::endl;
    }
#endif
  }
  return score;
}

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

inline int GetScoreForConsecutiveMatches(int consecutive_matches, int& previous_consecutive_matches) {
  if (previous_consecutive_matches == consecutive_matches) {
    return 0;
  }
  previous_consecutive_matches = consecutive_matches;

  std::vector<int> scores = { 0, 0, 50, 100, 150, 250, 350, 500, 750 };

  int score = (consecutive_matches >=9) ? 1000 : scores.at(consecutive_matches);

#if !defined(NDEBUG)
  if (score > 0) {
    std::cout << "GetScoreForConsecutiveMatches: " << score << std::endl;
  }
#endif

  return score;
}
