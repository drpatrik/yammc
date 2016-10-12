#pragma once

#include "constants.h"

inline int row_to_pixel(size_t row) { return (row * kSpriteHeight) + kBoardStartY; }
inline int col_to_pixel(size_t col) { return (col * kSpriteWidth) + kBoardStartX; }
inline int pixel_to_row(size_t y) { return (y <= kBoardStartY || y >= kBoardEndY) ? -1 : (y - kBoardStartY) / kSpriteHeight; }
inline int pixel_to_col(size_t x) { return (x <= kBoardStartX || x >= kBoardEndX) ? -1 : (x - kBoardStartX) / kSpriteWidth; }

class Position {
 public:
  Position() : row_(-1), col_(-1) {}
  Position(int row, int col) : row_(row), col_(col) {}

  int row() const { return row_; }
  int col() const { return col_; }

  int x() const { return col_to_pixel(col_); }
  int y() const { return row_to_pixel(row_); }

  bool operator<(const Position& rhs) const { return std::make_pair(row_, col_) < std::make_pair(rhs.row_, rhs.col_); }

  bool operator==(const Position& rhs) const { return std::make_pair(row_, col_) == std::make_pair(rhs.row_, rhs.col_); }

  bool operator==(const std::pair<int, int>& rhs) const { return std::make_pair(row_, col_) == rhs; }

  bool IsValid() const { return (-1 != row_ && -1 != col_); }

 private:
  int row_;
  int col_;
};
