#pragma once

#include "constants.h"

inline int row_to_pixel(size_t row) { return (row * kSpriteHeight) + kBoardStartY; }
inline int col_to_pixel(size_t col) { return (col * kSpriteWidth) + kBoardStartX; }
inline int pixel_to_row(size_t y) { return (y <= kBoardStartY || y >= kBoardEndY) ? -1 : (y - kBoardStartY) / kSpriteHeight; }
inline int pixel_to_col(size_t x) { return (x <= kBoardStartX || x >= kBoardEndX) ? -1 : (x - kBoardStartX) / kSpriteWidth; }

using Position = std::pair<int, int>;
