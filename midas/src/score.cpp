#include "score.h"

#include <set>
#include <fstream>
namespace {

const std::string kFilename("midas.shs");

std::pair<int, bool> GetScoreForTotalMatches(int total_matches, int& current_threshold_step) {
  int score = 0;
  bool threshold_reached = false;

  if (total_matches >= (current_threshold_step * kThresholdMultiplier)) {
    score = (500 + ((current_threshold_step - kInitialThresholdStep) * 250));
    current_threshold_step++;
    threshold_reached = true;

#if !defined(NDEBUG)
    if (score > 0) {
      std::cout << "GetScoreForTotalMatches: " << score << "N Jewels: " << total_matches << std::endl;
    }
#endif
  }
  return std::make_pair(score, threshold_reached);
}

int GetScoreForConsecutiveMatches(int consecutive_matches, int& previous_consecutive_matches) {
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

std::pair<int, bool> CalculateScore(size_t matches, int& total_matches, int& current_threshold_step,
                                    int consecutive_matches, int& previous_consecutive_matches) {
  int score = 0;
  int total_score = 0;
  bool threshold_reached;

  total_matches += static_cast<int>(matches);
  score += GetBasicScore(matches);
  score += GetScoreForConsecutiveMatches(consecutive_matches, previous_consecutive_matches);

  std::tie(total_score, threshold_reached) = GetScoreForTotalMatches(total_matches, current_threshold_step);

  return std::make_pair(score + total_score, threshold_reached);
}

}

ScoreManagement::ScoreManagement() {
  std::ifstream fs(kFilename);

  if (fs) {
    fs >> highscore_;
    displayed_highscore_ = highscore_;
  }
}

ScoreManagement::~ScoreManagement() {
  std::ofstream fs(kFilename);

  if (!fs) {
    std::cout << "Failed to save highscore to disk" << std::endl;
  } else {
    fs << highscore_ << std::endl;
  }
}

void ScoreManagement::Update(const std::vector<Position>& matches, int chains) {
  if (matches.size() == 0) {
    return;
  }
  consecutive_matches_ += chains;
  size_t unique_matches = std::set<Position>(matches.begin(), matches.end()).size();

  int score;
  bool threshold_reached;
  std::tie(score, threshold_reached) = CalculateScore(unique_matches, total_matches_, current_threshold_step_,
                                                      consecutive_matches_, previous_consecutive_matches_);

  if (threshold_reached) {
    threshold_reached_ = true;
  }
  score_ += score;
}
