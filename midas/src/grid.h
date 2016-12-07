#pragma once

#include "element.h"

#include <iostream>
#include <cassert>
#include <set>
#include <random>
#include <functional>

class Grid {
 public:
  Grid(int rows, int cols, AssetManagerInterface* am) : rows_(rows), cols_(cols), asset_manager_(am) {
    Generate();
  }

  // This constructor is only used by the test suit
  Grid(const std::vector<std::vector<int>>& grid, AssetManagerInterface* am)
      : rows_(grid.size()), cols_(grid.at(0).size()), asset_manager_(am) {
    grid_.resize(grid.size());
    for (int row = 0; row < rows_; row++) {
      grid_.at(row).resize(grid.at(0).size(), Element(0));
      for (int col = 0; col < cols_; col++) {
        grid_.at(row).at(col) = Element(static_cast<SpriteID>(grid.at(row).at(col)));
      }
    }
  }

  ~Grid() = default;

  inline int rows() const { return rows_; }

  inline int cols() const { return cols_; }

  bool IsFilling() const {
    if (!is_filling_) {
      return false;
    }
    const auto& last_row = grid_.at(rows_ - 1);

    is_filling_ = (std::count_if(std::begin(last_row), std::end(last_row), [](const Element &v) { return v == SpriteID::Empty; }) == cols_);

    return is_filling_;
  }

  inline const Element& At(int row, int col) const { return grid_.at(row).at(col); }

  inline Element& At(int row, int col) { return grid_.at(row).at(col); }

  inline const Element& At(const Position& p) const { return grid_.at(p.row()).at(p.col()); }

  inline Element& At(const Position& p) { return grid_.at(p.row()).at(p.col()); }

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
    grid_.resize(rows_, std::vector<Element>(cols_, Element(asset_manager_->GetSprite(SpriteID::Empty))));

    for (int row = 0; row < rows_; ++row) {
      for (int col = 0; col < cols_; ++col) {
        do {
          At(row, col) = Element(asset_manager_->GetSprite(static_cast<SpriteID>(distribution_(engine_))));
        } while(IsMatch(row, col));
      }
    }
    is_filling_ = true;
    fill_grid_ = std::move(grid_);

    grid_.clear();
    grid_.resize(rows_, std::vector<Element>(cols_, Element(asset_manager_->GetSprite(SpriteID::Empty))));
  }

  inline std::set<Position> GetAllMatches() const {
    return Matches(0, 0, rows_, cols_);
  }

  std::pair<std::vector<Position>, std::set<Position>> Collaps() {
    bool found = false;
    std::vector<Position> moved_objects;

    for (int row = rows_ - 1;row >= 1; --row) {
      for (int col = 0; col < cols_; ++col) {
        if (At(row, col).IsEmpty()) {
          std::swap(At(row, col), At(row - 1, col));
          if (!At(row, col).IsEmpty()) {
            moved_objects.push_back(Position(row, col));
          }
          found = true;
        }
      }
    }
    for (int col = cols_ - 1;col >= 0; --col) {
      if (At(0, col).IsEmpty()) {
        if (fill_grid_.empty()) {
          At(0, col) = Element(asset_manager_->GetSprite(static_cast<SpriteID>(distribution_(engine_))));
        } else {
          At(0, col) = fill_grid_.back().back();
          fill_grid_.back().pop_back();
          if (fill_grid_.back().empty()) {
            fill_grid_.pop_back();
          }
        }
        moved_objects.push_back(Position(0, col));
      }
    }
    std::set<Position> matches;

    if (!found) {
      matches = GetAllMatches();
    }
    return std::make_pair(moved_objects, matches);
  }

  std::set<Position> GetMatchesFromSwap(const Position& p1, const Position& p2) {
    std::swap(At(p1), At(p2));

    auto matches = GetAllMatches();

    std::swap(At(p1), At(p2));
    return matches;
  }

  void Render(SDL_Renderer *renderer) const {
    for (int row = 0; row < rows_; ++row) {
      for (int col = 0; col < cols_; ++col) {
        At(row, col).Render(renderer, col_to_pixel(col), row_to_pixel(row));
      }
    }
  }

  void Print() const {
    for (auto row = 0; row < rows_; ++row) {
      for (auto col = 0; col < cols_; ++col) {
        std::cout << At(row, col).id() << '\t';
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }

 protected:
  using GetValue = std::function<Element(int i)>;
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
    const auto& value = At(row, col);

    for (int i = start - 1; i >= 0; i--) {
      if (value_at(i) != value || value_at(i).IsEmpty()) {
        break;
      }
      matches.emplace(pos(i));
    }
    // check upper bounds
    for (int i = start + 1; i < end; i++) {
      if (value_at(i) != value || value_at(i).IsEmpty()) {
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
    auto value_at = [this, col](int row) { return At(row, col); };
    auto pos = [col](int row) { return Position(row, col); };

    return Matches(row, col, row, rows_, value_at, pos);
  }

  std::set<Position> GetHorizontalMatches(int row, int col) const {
    auto value_at = [this, row](int col) { return At(row, col); };
    auto pos = [row](int col) { return Position(row, col); };

    return Matches(row, col, col, cols_, value_at, pos);
  }

 private:
  int rows_;
  int cols_;
  mutable bool is_filling_ = true;
  std::mt19937 engine_ {std::random_device{}()};
  std::uniform_int_distribution<int> distribution_{ 0, kNumSprites - 1 };
  std::vector<std::vector<Element>> grid_;
  std::vector<std::vector<Element>> fill_grid_;
  AssetManagerInterface* asset_manager_ = nullptr;
};
