#include <iostream>
#include "constants.h"
#include "asset_manager.h"

const std::string kAssetFolder = "../../assets/";

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name) {
  std::string full_path =  kAssetFolder + name;

  SDL_Surface* surface = SDL_LoadBMP(full_path.c_str());
  if (nullptr == surface) {
    std::cout << "Failed to load surface " << full_path
              << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}

TTF_Font *LoadFont(const std::string& name, int size) {
  std::string full_path = kAssetFolder + name;

  TTF_Font *font = TTF_OpenFont(full_path.c_str(), size);

  if (font == nullptr) {
    std::cout << "Failed to load text " << full_path
              << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }

  return font;
}

AssetManager::AssetManager(SDL_Renderer *renderer) {
  std::vector<SpriteName> names_ {Blue, Green, Red, Yellow, Purple};
  std::vector<std::string> sprites {"Blue.bmp", "Green.bmp", "Red.bmp", "Yellow.bmp", "Purple.bmp"};
  std::vector<std::string> selected {"BlueSelected.bmp", "GreenSelected.bmp", "RedSelected.bmp", "YellowSelected.bmp", "PurpleSelected.bmp"};

  for (size_t i = 0; i < sprites.size(); ++i) {
    sprites_.push_back(std::make_shared<Sprite>(names_[i], LoadTexture(renderer, sprites[i]), LoadTexture(renderer, selected[i])));
  }
  sprites_.push_back(std::make_shared<Sprite>(Empty, nullptr, nullptr));
  std::vector<std::string> fonts {"Cabin-Regular.ttf", "Cabin-Bold.ttf"};

  for (auto& f:fonts) {
    fonts_.push_back(LoadFont(f, kFontSize));
  }
  background_texture_ = LoadTexture(renderer, "BackGround.bmp");
}

AssetManager::~AssetManager() noexcept {
  std::for_each(std::begin(fonts_), std::end(fonts_), [] (auto font) { TTF_CloseFont(font); });
  SDL_DestroyTexture(background_texture_);
}
