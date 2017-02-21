#pragma once

#include "timer.h"

#include <cstddef>

const int kWidth = 755;
const int kHeight = 600;
const double kFPS = 60;
const int kGameTime = 180;
const int kFontSize = 25;
const int kScoreFontSize = 15;
const int kSpriteWidth = 38;
const int kSpriteHeight = 38;
const size_t kNumSprites = 5; // Red, Blue, Green, Yellow and Purple
const size_t kMatchNumber = 3;
const int kRows = 9;
const int kCols = 8;
const int kShowHintTimer = 10;
const int kIdlePenaltyTimer = 5;
const int kInitialThresholdStep = 1;
const int kThresholdMultiplier = 100;
const size_t kBoardStartX = 340;
const size_t kBoardEndX = kBoardStartX + (kCols * kSpriteWidth);
const size_t kBoardStartY = 95;
const size_t kBoardEndY = kBoardStartY + (kRows * kSpriteHeight);
