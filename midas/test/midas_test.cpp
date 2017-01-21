#include <gtest/gtest.h>
#include <initializer_list>
#include "grid.h"

class AssetManagerMock : public AssetManagerInterface {
 public:
  virtual SDL_Texture *GetBackgroundTexture() override { return nullptr; }
  virtual std::shared_ptr<Sprite> GetSprite(int) override {
    return std::make_shared<Sprite>(SpriteID::Blue);
  }
  virtual std::shared_ptr<Sprite> GetSprite() override {
    return std::make_shared<Sprite>(SpriteID::Blue);
  }
  virtual std::shared_ptr<Sprite> GetSprite(SpriteID id) override {
    return std::make_shared<Sprite>(id);
  }
  virtual std::vector<SDL_Texture*> GetStarTextures() override {
    return std::vector<SDL_Texture*>();
  }
  virtual std::vector<SDL_Texture*> GetExplosionTextures() override {
    return std::vector<SDL_Texture*>();
  }
  virtual SDL_Texture * GetSpriteAsTexture(SpriteID) override { return nullptr; }
  virtual TTF_Font *GetFont(int) override { return nullptr; }
  virtual void ResetPreviousIds() override {}
};

AssetManagerMock kAssetManagerMock;

TEST(MidasTest, FindAllMatches) {
  std::vector<std::vector<int>> init_grid {
    {0,   0,  0,  3,  4,  7,  7, 7},  // 0
    { 8,  9, 10, 11, 12, 13, 14, 15}, // 1
    {16, 17, 18, 19, 20, 21, 22, 23}, // 2
    {24, 25, 26, 27, 28, 29, 30, 31}, // 3
    {32, 33, 34, 28, 28, 28, 38, 39}, // 4
    {40, 41, 42, 43, 28, 45, 46, 47}, // 5
    {48, 49, 50, 51, 52, 53, 54, 55}, // 6
    {56, 56, 56, 59, 60, 61, 61, 61}, // 7
    {0, 1, 0, 1, 0, 1, 0, 0}          // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  auto matches = grid.GetAllMatches();

  ASSERT_EQ(matches.size(), 17ul);
}

TEST(MidasTest, TestIsMatch) {
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

  ASSERT_TRUE(grid.IsMatch(0,1));
  ASSERT_TRUE(grid.IsMatch(3,1));
  ASSERT_TRUE(grid.IsMatch(4,1));
  ASSERT_TRUE(grid.IsMatch(5,1));
  ASSERT_TRUE(grid.IsMatch(7,7));
  ASSERT_FALSE(grid.IsMatch(7,3));
  ASSERT_FALSE(grid.IsMatch(2,6));
}

TEST(MidasTest, FindNoSolution) {
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

  ASSERT_EQ(matches.size(), 0ul);
}

TEST(MidasTest, FindSolutionAfterSwap) {
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

  ASSERT_EQ(matches.size(), 0ul);

  ASSERT_TRUE(!grid.GetMatchesFromSwap(Position(4, 4), Position(4, 5)).empty());
}

// The logic has changed and Collopase no longer
// removes the matched elements. Need to add a function
// to do that to make this test fly again
TEST(MidasTest, DISABLED_CollapseTest) {
  std::vector<std::vector<int>> init_grid {
    { 0,  1,  2,  3,    4,  6,  7, 7},   // 0
    { 8,  9, 10, 11,   12, 13,  14, 15}, // 1
    {16, 17, 18, 19,   20, 21, 22, 23},  // 2
    {24, 25, 26, 1, 1, 29, 30, 31},      // 3
    {32, 33, 28, 1, 1, 28, 38, 39},      // 4
    {40, 41, 42, 1, 1, 45, 46, 47},      // 5
    { 1,  1,  1, 51, 52,  53, 54, 55},   // 6
    {56, 57, 58, 59,   60, 61, 62, 63},  // 7
    {0, 0, 0, 0, 0, 0, 0, 0}             // 8
  };
  Grid grid(init_grid, &kAssetManagerMock);

  ASSERT_EQ(grid.GetAllMatches().size(), 9u);
  auto r = grid.Collaps();
  std::cout << std::get<1>(r).size() << std::endl;
  ASSERT_TRUE(std::get<1>(r).empty());
  grid.Collaps();
  grid.Collaps();
  grid.Collaps();
  grid.Collaps();
  ASSERT_TRUE(grid.At(3,3).id() == static_cast<SpriteID>(3));
  ASSERT_TRUE(grid.At(3,4).id() == static_cast<SpriteID>(4));
  ASSERT_TRUE(grid.At(5,3).id() == static_cast<SpriteID>(19));
  ASSERT_TRUE(grid.At(5,4).id() == static_cast<SpriteID>(20));
}

TEST(MidasTest, DISABLED_Generate) {
  Grid grid(kRows, kCols, &kAssetManagerMock);

  for (int i  = 0;i < 1000000;i++) {
    grid.Generate();
    ASSERT_TRUE(grid.GetAllMatches().empty());
  }
}
