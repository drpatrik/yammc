#include "asset_manager.h"

#include <iostream>

namespace {

const size_t kStarTextures = 12;
const size_t kExplosionTextures = 17;

const std::string kAssetFolder = "../../assets/";

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name) {
  std::string full_path =  kAssetFolder + "art/" + name;

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
  std::string full_path = kAssetFolder + "fonts/" + name;

  TTF_Font *font = TTF_OpenFont(full_path.c_str(), size);

  if (font == nullptr) {
    std::cout << "Failed to load text " << full_path
              << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }

  return font;
}

std::vector<SDL_Texture*> LoadTextures(SDL_Renderer *renderer, const std::string& name, size_t n) {
  std::vector<SDL_Texture*> textures;

  for (size_t i = 1; i <= n; ++i) {
    std::string fullname = name + "_" + std::to_string(i) + ".bmp";
    textures.push_back(LoadTexture(renderer, fullname));
  }

  return textures;
}

}

AssetManager::AssetManager(SDL_Renderer *renderer) {
  std::vector<SpriteID> ids_ { Blue, Green, Red, Yellow, Purple };
  std::vector<std::string> sprites { "Blue.bmp", "Green.bmp", "Red.bmp", "Yellow.bmp", "Purple.bmp" };
  std::vector<std::string> selected { "BlueSelected.bmp", "GreenSelected.bmp", "RedSelected.bmp", "YellowSelected.bmp", "PurpleSelected.bmp" };

  for (size_t i = 0; i < sprites.size(); ++i) {
    sprites_.push_back(std::make_shared<Sprite>(ids_[i], LoadTexture(renderer, sprites[i]), LoadTexture(renderer, selected[i])));
  }
  sprites_.push_back(std::make_shared<Sprite>(Empty, nullptr, nullptr));

  star_textures_ = LoadTextures(renderer, "star", kStarTextures);
  explosion_texture_ = LoadTextures(renderer, "explosion", kExplosionTextures);

  std::vector<std::pair<std::string, int>> fonts {
    std::make_pair("Cabin-Regular.ttf", kFontSize),
    std::make_pair("Cabin-Bold.ttf", kFontSize),
    std::make_pair("Cabin-Regular.ttf", kScoreFontSize)
  };

  for (const auto& f:fonts) {
    fonts_.push_back(LoadFont(f.first, f.second));
  }
  background_texture_ = LoadTexture(renderer, "BackGround.bmp");
}

AssetManager::~AssetManager() noexcept {
  std::for_each(std::begin(fonts_), std::end(fonts_), [] (auto font) { TTF_CloseFont(font); });
  std::for_each(std::begin(star_textures_), std::end(star_textures_), [] (auto texture) { SDL_DestroyTexture(texture); });
  std::for_each(std::begin(explosion_texture_), std::end(explosion_texture_), [] (auto texture) { SDL_DestroyTexture(texture); });
  SDL_DestroyTexture(background_texture_);
}
