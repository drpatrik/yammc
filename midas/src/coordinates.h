#pragma once

const size_t kBoardStartX = 340;
const size_t kBoardEndX = 655;
const size_t kBoardStartY = 95;
const size_t kBoardEndY = 425;

inline int row_to_pixel(size_t row) { return (row * 40) + kBoardStartY; }
inline int col_to_pixel(size_t col) { return (col * 40) + kBoardStartX; }

inline int pixel_to_row(size_t y) { return (y < kBoardStartY || y > kBoardEndY) ? -1 : (y - kBoardStartY) / 40; }
inline int pixel_to_col(size_t x) { return (x < kBoardStartX || x > kBoardEndX) ? -1 : (x - kBoardStartX) / 40; }

using Position = std::pair<int, int>;
