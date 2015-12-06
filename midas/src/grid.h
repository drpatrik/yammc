#pragma once

#include <iostream>
#include <cassert>
#include <set>
#include <random>
#include <functional>

#include "constants.h"
#include "coordinates.h"

template <typename T>
class Grid {
 public:
  Grid() : Grid(kRows, kCols) {}

  Grid(int rows, int cols) : rows_(rows), cols_(cols) {
    Generate();
  }

  Grid(const std::vector<std::vector<T>>& grid)
      : rows_(grid.size()), cols_(grid.at(0).size()), grid_(grid) {}

  ~Grid() = default;

  inline int rows() const { return rows_; }

  inline int cols() const { return cols_; }

  inline const T& At(int row, int col) const {
    return grid_.at(row).at(col);
  }

  inline T& At(int row, int col) { return grid_.at(row).at(col); }

  inline const T& At(const Position& p) const {
    return grid_.at(p.first).at(p.second);
  }

  inline T& At(const Position& p) { return grid_.at(p.first).at(p.second); }

  void for_each(std::function<void(int row, int col, int id)> eval) {
    for (auto row = 0;row < rows_; ++row) {
      for (auto col = 0;col < cols_; ++col) {
        eval(row, col, At(row, col));
      }
    }
  }

  // Can only be used for checking a match when generating a
  // new board
  bool IsMatch(int row, int col) const {
    assert(kMatchNumber <= 3);
    // Look behind
    int match_row = (row - 1 >= 0) && At(row - 1, col) == At(row, col);
    match_row += (row - 2 >= 0) && At(row - 2, col) == At(row, col);
    // Look in front
    match_row += (row + 1 < kRows) && At(row + 1, col) == At(row, col);
    match_row += (row + 2 < kRows) && At(row + 2, col) == At(row, col);
    // Look above
    int match_col = (col - 1 >= 0) && At(row, col - 1) == At(row, col);
    match_col += (col - 2 >= 0) && At(row, col - 2) == At(row, col);
    // Look Below
    match_col += (col + 1 < kCols) && At(row, col + 1) == At(row, col);
    match_col += (col + 2 < kCols) && At(row, col + 2) == At(row, col);

    return (match_row >= 2) || (match_col >= 2);
  }

  void Generate() {
    grid_.resize(rows_, std::vector<T>(cols_, T(kEmptyObject)));
    for (auto row = 0; row < rows_; ++row) {
      for (auto col = 0; col < cols_; ++col) {
        do {
          At(row, col) = T(distribution_(engine_));
        } while(IsMatch(row, col));
      }
    }
  }

  inline std::set<Position> GetAllMatches() const {
    return Matches(0, 0, rows_, cols_);
  }

  std::pair<bool,int> Collaps(std::vector<Position>& p) {
    bool found = false;

    for (auto col = 0;col < cols_; ++col) {
      if (At(0, col) == kEmptyObject) {
        At(0, col) = T(distribution_(engine_));
      }
    }
    for (auto row = rows_ - 1;row >= 1; --row) {
      for (auto col = 0; col < cols_; ++col) {
        if (At(row, col) == kEmptyObject) {
          std::swap(At(row, col), At(row - 1, col));
          p.push_back(std::make_pair(row, col));
          found = true;
        }
      }
    }
    int match_count = 0;

    if (!found) {
      auto matches = GetAllMatches();

      match_count += matches.size();

      for (auto& m : matches) {
        At(m) = kEmptyObject;
      }
    }
    return std::make_pair(found, match_count);
  }

  std::set<Position> Switch(const Position& p1, const Position& p2) {
    std::swap(At(p1), At(p2));

    auto matches = GetAllMatches();

    std::swap(At(p1), At(p2));
    return matches;
  }

  void Print() const {
    for (auto row = 0; row < rows_; ++row) {
      for (auto col = 0; col < cols_; ++col) {
        std::cout << At(row, col) << '\t';
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }

 protected:
  using GetValue = std::function<T(int i)>;
  using GetPosition = std::function<Position(int i)>;

  std::set<Position> Matches(int start_row, int start_col, int rows, int cols) const {
    std::set<Position> matches;

    for (auto row = start_row; row < rows; ++row) {
      for (auto col = start_col; col < cols; ++col) {
        auto vertical = GetVerticalMatches(row, col);

        matches.insert(std::begin(vertical), std::end(vertical));

        auto horizontal = GetHorizontalMatches(row, col);

        matches.insert(std::begin(horizontal), std::end(horizontal));
      }
    }
    return matches;
  }

  std::set<Position> Matches(int row, int col, int start, int end, GetValue value_at, GetPosition pos) const {
    std::set<Position> matches;

    matches.emplace(row, col);
    // check lower bounds
    auto value = At(row, col);

    for (int i = start - 1; i >= 0; i--) {
      if (value_at(i) != value || value_at(i) == kEmptyObject) {
        break;
      }
      matches.emplace(pos(i));
    }
    // check upper bounds
    for (int i = start + 1; i < end; i++) {
      if (value_at(i) != value || value_at(i) == kEmptyObject) {
        break;
      }
      matches.emplace(pos(i));
    }
    if (matches.size() < kMatchNumber) {
      matches.clear();
    }
    return matches;
  }

  std::set<Position> GetVerticalMatches(int row, int col) const {
    auto value_at = [this, col](int i) { return At(i, col); };
    auto pos = [col](int i) { return std::make_pair(i, col); };

    return Matches(row, col, row, rows_, value_at, pos);
  }

  std::set<Position> GetHorizontalMatches(int row, int col) const {
    auto value_at = [this, row](int i) { return At(row, i); };
    auto pos = [row](int i) { return std::make_pair(row, i); };

    return Matches(row, col, col, cols_, value_at, pos);
  }

 private:
  int rows_;
  int cols_;
  std::mt19937 engine_ {std::random_device{}()};
  std::uniform_int_distribution<int> distribution_{0, kNumObjects - 1};
  std::vector<std::vector<T>> grid_;
};
