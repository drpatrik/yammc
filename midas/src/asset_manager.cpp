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
  std::vector<std::string> objects {"Blue.bmp", "Green.bmp", "Red.bmp", "Yellow.bmp", "Purple.bmp", "BackGround.bmp"};

  for (auto& obj:objects) {
    textures_.push_back(LoadTexture(renderer, obj));
  }
  std::vector<std::string> fonts {"Arial.ttf", "ArialBold.ttf"};

  for (auto& f:fonts) {
    fonts_.push_back(LoadFont(f, kFontSize));
  }

}

AssetManager::~AssetManager() noexcept {
  std::for_each(std::begin(textures_), std::end(textures_), [] (SDL_Texture *texture) { SDL_DestroyTexture(texture); });
  std::for_each(std::begin(fonts_), std::end(fonts_), [] (TTF_Font *font) { TTF_CloseFont(font); });
}
