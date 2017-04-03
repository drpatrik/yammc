#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <initializer_list>
#include "animation.h"
#include "grid.h"

class AssetManagerMock : public AssetManagerInterface {
 public:
  virtual std::shared_ptr<const Sprite> GetSprite() const override {
    return std::make_shared<const Sprite>(SpriteID::Blue);
  }

  virtual std::shared_ptr<const Sprite> GetSprite(int) const override {
    return std::make_shared<const Sprite>(SpriteID::Blue);
  }

  virtual std::shared_ptr<const Sprite> GetSprite(SpriteID id) const override {
    return std::make_shared<const Sprite>(id);
  }

  virtual void ResetPreviousIds() override {}
};

AssetManagerMock kAssetManagerMock;

int consecutive_matches = 0;
int previous_consecutive_matches = -1;

TEST_CASE("FindAllMatchesAndChains") {
  std::vector<std::vector<int>> init_grid {
    {0,   0,  0,  3,  4,  7,  7, 7},  // 0 // 1 chains (0H) 7(h)
    { 8,  9, 10, 11, 12, 13,  7, 15}, // 1
    {16, 17, 18, 19, 20, 21,  7, 23}, // 2 // chains (7V)
    {24, 25, 26, 27, 28, 29, 30, 31}, // 3
    {32, 33, 34, 28, 28, 28, 38, 39}, // 4 // 1 chain (28H)
    {40, 41, 42, 43, 28, 45, 46, 47}, // 5 // 1 chain (28V)
    {48, 49, 50, 51, 52, 53, 54, 55}, // 6
    {56, 56, 56, 59, 60, 61, 61, 61}, // 7 2 chains (56H, 61H)
    {0, 1, 0, 1, 0, 1, 0, 0}          // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  std::vector<Position> matches;
  int chains;

  std::tie(matches, chains) = grid.GetAllMatches();

  REQUIRE(matches.size() == 21ul);
  REQUIRE(chains == 7);
}

/*TEST_CASE("FindAllMatchesAndChains2") {
  std::vector<std::vector<int>> init_grid {
    {6, 6, 6, 6, 6, 6, 6, 6},  // 6 // 1 chains (6H) 7(h)
    {6, 6, 6, 6, 6, 6, 6, 6}, // 1
    {0, 0, 1, 1, 1, 1, 6, 6}, // 2 // chains (7V)
    {6, 6, 6, 6, 6, 6, 6, 6}, // 3
    {6, 6, 6, 6, 6, 6, 6, 6}, // 4 // 1 chain (28H)
    {6, 6, 6, 6, 6, 6, 6, 6}, // 5 // 1 chain (28V)
    {6, 6, 6, 6, 6, 6, 6, 6}, // 6
    {6, 6, 6, 6, 6, 6, 6, 6}, // 7 2 chains (56H, 61H)
    {6, 1, 6, 1, 6, 1, 6, 6}          // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  std::vector<Position> matches;
  int chains;

  std::tie(matches, chains) = grid.GetAllMatches();

  std::cout << chains << std::endl;
  }*/

TEST_CASE("TestIsMatch") {
  std::vector<std::vector<int>> init_grid {
    {0,   0,  0,  3,  4,  5,  6, 7},  // 0
    { 8,  9, 10, 11, 12, 13, 14, 15}, // 1
    {16, 17, 18, 19, 20, 21, 22, 22}, // 2
    {24, 25, 26, 27, 28, 29, 30, 31}, // 3
    {32, 25, 34, 28, 28, 28, 38, 39}, // 4
    {40, 25, 42, 43, 28, 45, 46, 47}, // 5
    {48, 49, 50, 51, 52, 53, 54, 55}, // 6
    {56, 56, 56, 59, 60, 61, 61, 61}, // 7
    {0, 0, 0, 0, 0, 0, 0, 0}          // 8
  };

  Grid grid(init_grid, &kAssetManagerMock);

  REQUIRE(grid.IsMatch(0,1));
  REQUIRE(grid.IsMatch(3,1));
  REQUIRE(grid.IsMatch(4,1));
  REQUIRE(grid.IsMatch(5,1));
  REQUIRE(grid.IsMatch(7,7));
  REQUIRE(grid.IsMatch(7,3) == false);
  REQUIRE(grid.IsMatch(2,6) == false);
}

TEST_CASE("FindNoSolution") {
  std::vector<std::vector<int>> init_grid {
    {0,   1,  2,  3,  4,  5,  7, 7},  // 0
    { 8,  9, 10, 11, 12, 13,  7, 15}, // 1
    {16, 17, 18, 19, 20, 21, 22, 23}, // 2
    {24, 25, 26, 27, 28, 29, 30, 31}, // 3
    {32, 33, 34, 35, 36, 37, 38, 39}, // 4
    {40, 41, 42, 43, 28, 45, 46, 47}, // 5
    {48, 49, 50, 51, 52, 53, 54, 55}, // 6
    {56, 57, 58, 59, 60, 61, 62, 63}, // 7
    {0, 1, 0, 1, 0, 1, 0, 0}          // 8
  };

  Grid grid(init_grid, &kAssetManagerMock);

  auto matches = grid.GetAllMatches();

  REQUIRE(matches.first.size() == 0ul);
}

TEST_CASE("FindSolutionAfterSwap") {
  std::vector<std::vector<int>> init_grid {
    {0,   1,  2,  3,  4,  5,  7, 7}, // 0
    { 8,  9, 10, 11, 12, 13,  7, 15}, // 1
    {16, 17, 18, 19, 20, 21, 22, 23}, // 2
    {24, 25, 26, 27, 28, 29, 30, 31}, // 3
    {32, 33, 28, 28, 36, 28, 38, 39}, // 4
    {40, 41, 42, 43, 28, 45, 46, 47}, // 5
    {48, 49, 50, 51, 52, 53, 54, 55}, // 6
    {56, 57, 58, 59, 60, 61, 62, 63}, // 7
    {0, 1, 0, 1, 0, 1, 0, 0}          // 8
  };

  Grid grid(init_grid, &kAssetManagerMock);

  auto matches = grid.GetAllMatches();

  REQUIRE(matches.first.size() == 0ul);

  REQUIRE(!grid.GetMatchesFromSwap(Position(4, 4), Position(4, 5)).first.empty());
}

void RemoveMatches(Grid& grid, const std::vector<Position>& matches) {
  for (const auto& match : matches) {
    grid.At(match) = Element(SpriteID::Empty);
  }
}

TEST_CASE("CollapseTest") {
  std::vector<std::vector<int>> init_grid {
    { 0,  1,  2,  3,  4,  6,  7,  7},  // 0
    { 8,  9, 10, 11, 12, 13, 14, 15},  // 1
    {16, 17, 18, 19, 20, 21, 22, 23},  // 2
    {24, 25, 26,  1,  1, 29, 30, 31},  // 3
    {32, 33, 28,  1,  1, 28, 38, 39},  // 4
    {40, 41, 42,  1,  1, 45, 46, 47},  // 5
    { 1,  1,  1, 51, 52, 53, 54, 55},  // 6
    {56, 57, 58, 59, 60, 61, 62, 63},  // 7
    { 1,  2,  1,  2,  1,  2,  1,  2}   // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  REQUIRE(grid.GetAllMatches().first.size() == 9u);
  RemoveMatches(grid, grid.GetAllMatches().first);
  REQUIRE(grid.GetAllMatches().first.size() == 0u);

  grid.Collaps(consecutive_matches, previous_consecutive_matches);
  grid.Collaps(consecutive_matches, previous_consecutive_matches);
  grid.Collaps(consecutive_matches, previous_consecutive_matches);
  REQUIRE(grid.At(3,3).id() == static_cast<SpriteID>(3));
  REQUIRE(grid.At(3,4).id() == static_cast<SpriteID>(4));
  REQUIRE(grid.At(5,3).id() == static_cast<SpriteID>(19));
  REQUIRE(grid.At(5,4).id() == static_cast<SpriteID>(20));
}

TEST_CASE("FindPotentialMatches") {
  std::vector<std::vector<int>> init_grid {
    {10, 12, 14, 18, 20, 22, 23, 43}, // 0
    { 8,  9, 10, 11, 12, 13,  7, 15}, // 1
    {16, 17, 18, 19, 28, 21, 22, 23}, // 2
    {24, 25, 26, 27, 42, 29, 30, 31}, // 3
    {32, 33, 28, 28, 36, 43, 38, 39}, // 4
    {40, 41, 36, 36, 28, 45, 46, 47}, // 5
    {48, 49, 50, 51, 36, 53, 54, 55}, // 6
    {56, 57, 58, 59, 36, 61, 62, 63}, // 7
    { 0,  1,  0,  1,  0,  1,  0,  0}  // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  bool matches_found;
  std::pair<Position, Position> swap_to_match;

  std::tie(matches_found, swap_to_match) = grid.FindPotentialMatches();

  REQUIRE(matches_found);

  auto matches = grid.GetMatchesFromSwap(swap_to_match.first, swap_to_match.second);

  REQUIRE(matches.first.size() == 9lu);

  std::vector<std::vector<int>> init_grid2 {
    {0, 1, 2, 3, 0, 1, 2, 3},
    {3, 0, 1, 2, 3, 0, 1, 2},
    {2, 3, 0, 1, 2, 3, 0, 1},
    {1, 2, 3, 0, 1, 2, 3, 0},
    {3, 1, 2, 3, 0, 1, 2, 3},
    {2, 3, 1, 2, 3, 0, 1, 2},
    {1, 2, 3, 1, 2, 3, 0, 1},
    {0, 1, 2, 3, 1, 2, 3, 0},
    {3, 0, 1, 2, 3, 1, 2, 3}
  };
  Grid grid_no_matches(init_grid2, &kAssetManagerMock);

  std::tie(matches_found, swap_to_match) = grid_no_matches.FindPotentialMatches();

  REQUIRE(matches_found == false);
}

TEST_CASE("DISABLED_TestFindPositionForScoreAnimation") {
  std::vector<std::vector<int>> init_grid {
    {5, 5, 5, 5, 5, 5, 5, 5}, // 0
    {5, 1, 1, 1, 5, 5, 5, 5}, // 1
    {5, 5, 5, 1, 5, 5, 5, 5}, // 2
    {5, 5, 5, 1, 5, 5, 5, 5}, // 3
    {5, 5, 5, 5, 5, 5, 5, 5}, // 4
    {5, 5, 5, 5, 5, 5, 5, 5}, // 5
    {5, 5, 5, 5, 5, 5, 5, 5}, // 6
    {5, 5, 5, 5, 5, 5, 5, 5}, // 7
    {5, 5, 5, 5, 5, 5, 5, 5}, // 8

  };
  Grid grid(init_grid, &kAssetManagerMock);

  std::vector<Position> matches;
  int chains;

  std::tie(matches, chains) = grid.GetAllMatches();

  // std::cout << matches.size() << std::endl;

  //FindPositionForScoreAnimation(matches, chains).Print();
}
