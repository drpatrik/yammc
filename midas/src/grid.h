#pragma once

#include "element.h"
#include "coordinates.h"

#include <iostream>
#include <set>
#include <functional>

class Grid {
 public:
  enum class GenerateType { Fill, NoFill };

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
    static_assert(kMatchNumber <= 3, "IsMatch assumes that a match is exactly 3");
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

  void Generate(GenerateType type = GenerateType::Fill) {
    do {
      grid_.clear();
      grid_.resize(rows_, std::vector<Element>(cols_, Element(asset_manager_->GetSprite(SpriteID::Empty))));
      for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
          do {
            At(row, col) = Element(asset_manager_->GetSprite());
          } while(IsMatch(row, col));
        }
      }
    } while (!FindPotentialMatches().first);

    if (GenerateType::Fill == type) {
        is_filling_ = true;
        fill_grid_ = grid_;
        grid_.clear();
        grid_.resize(rows_, std::vector<Element>(cols_, Element(asset_manager_->GetSprite(SpriteID::Empty))));
    }
  }

  inline std::set<Position> GetAllMatches() const {
    return Matches(0, 0, rows_, cols_);
  }

  std::pair<std::vector<Position>, std::set<Position>> Collaps() {
    bool grid_is_unstable = false;
    std::vector<Position> moved_objects;

    for (int row = rows_ - 1;row >= 1; --row) {
      for (int col = 0; col < cols_; ++col) {
        if (At(row, col).IsEmpty()) {
          std::swap(At(row, col), At(row - 1, col));
          if (!At(row, col).IsEmpty()) {
            moved_objects.push_back(Position(row, col));
          }
          grid_is_unstable = true;
        }
      }
    }
    for (int col = cols_ - 1;col >= 0; --col) {
      if (At(0, col).IsEmpty()) {
        bool filling = !fill_grid_.empty();

        At(0, col) = (filling) ? fill_grid_.back().back() : Element(asset_manager_->GetSprite(col));
        if (filling) {
          fill_grid_.back().pop_back();
          if (fill_grid_.back().empty()) {
            fill_grid_.pop_back();
          }
        }
        grid_is_unstable = true;
        grid_is_dirty_ = !filling;
        moved_objects.push_back(Position(0, col));
      }
    }
    std::set<Position> matches;

    if (!grid_is_unstable && grid_is_dirty_) {
      asset_manager_->ResetPreviousIds();
      matches = GetAllMatches();
      if (matches.size() == 0 && !FindPotentialMatches().first) {
        Generate(Grid::GenerateType::NoFill);
        std::cout << "No solutions found, creating a new board" << std::endl;
      }
      grid_is_dirty_ = false;
    }
    return std::make_pair(moved_objects, matches);
  }

  std::set<Position> GetMatchesFromSwap(const Position& p1, const Position& p2) {
    std::swap(At(p1), At(p2));

    auto matches = GetAllMatches();

    std::swap(At(p1), At(p2));
    return matches;
  }

  std::pair<bool, std::pair<Position, Position>> FindPotentialMatches() {
    std::set<Position> matches;
    std::pair<Position, Position> positions;

    for (int row = 0; row < rows_; ++row) {
      for (int col = 0; col < cols_; ++col) {
        if (row + 1 < rows_) {
          positions.first = Position(row, col);
          positions.second =  Position(row + 1, col);

          matches = GetMatchesFromSwap(positions.first, positions.second);
        }
        if (matches.empty() && col + 1 < cols_) {
          positions.first = Position(row, col);
          positions.second =  Position(row, col + 1);

          matches = GetMatchesFromSwap(positions.first, positions.second);
        }
        if (!matches.empty()) {
          return std::make_pair(true, positions);
        }
      }
    }

    return std::make_pair(false, positions);
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
        auto column_matches = GetColumnMatches(row, col);

        matches.insert(std::begin(column_matches), std::end(column_matches));

        auto row_matches = GetRowMatches(row, col);

        matches.insert(std::begin(row_matches), std::end(row_matches));
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

  std::set<Position> GetColumnMatches(int row, int col) const {
    auto value_at = [this, col](int row) { return At(row, col); };
    auto pos = [col](int row) { return Position(row, col); };

    return Matches(row, col, row, rows_, value_at, pos);
  }

  std::set<Position> GetRowMatches(int row, int col) const {
    auto value_at = [this, row](int col) { return At(row, col); };
    auto pos = [row](int col) { return Position(row, col); };

    return Matches(row, col, col, cols_, value_at, pos);
  }

 private:
  int rows_;
  int cols_;
  mutable bool is_filling_ = true;
  bool grid_is_dirty_ = false;
  std::vector<std::vector<Element>> grid_;
  std::vector<std::vector<Element>> fill_grid_;
  AssetManagerInterface* asset_manager_ = nullptr;
};
