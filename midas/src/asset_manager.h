#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

enum Font{ Normal, Bold };

class AssetManager {
 public:
  AssetManager(SDL_Renderer *renderer);
  AssetManager(const AssetManager&) = delete;
  ~AssetManager() noexcept;

  SDL_Texture* GetBackgroundTexture() { return textures_[textures_.size() - 1];}
  SDL_Texture* GetTexture(int id) { return (id < int(textures_.size() - 1)) ? textures_[id] : nullptr; }
  TTF_Font* GetFont(int id) { return fonts_[id]; }

 private:
  std::vector<TTF_Font*> fonts_;
  std::vector<SDL_Texture*> textures_;
};
