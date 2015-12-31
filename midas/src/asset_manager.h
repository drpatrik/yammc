#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

enum Font{ Normal, Bold };
enum SpriteName{ Blue, Green, Red, Yellow, Purple, Empty, OwnedByAnimation };

inline void SwapSpriteName(SpriteName& a, SpriteName& b) {
  SpriteName tmp = b;

  b = a;
  a = tmp;
};


class AssetManager {
 public:
  struct Sprite {
    Sprite(SpriteName name, SDL_Texture* sprite, SDL_Texture* selected) : name_(name), sprite_(sprite), selected_(selected) {}
    ~Sprite() { SDL_DestroyTexture(sprite_); SDL_DestroyTexture(selected_); }
    auto operator()() const { return sprite_; }
    SpriteName name_;
    SDL_Texture* sprite_;
    SDL_Texture* selected_;
  };

  AssetManager(SDL_Renderer *renderer);
  AssetManager(const AssetManager&) = delete;
  ~AssetManager() noexcept;

  SDL_Texture* GetBackgroundTexture() { return background_texture_;}
  Sprite& GetSprite(int id) { return *sprites_[id]; }
  TTF_Font* GetFont(int id) { return fonts_[id]; }

 private:
  std::vector<TTF_Font*> fonts_;
  std::vector<std::shared_ptr<Sprite>> sprites_;
  SDL_Texture *background_texture_;
};
