#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

enum Font { Normal, Bold };
enum SpriteID { Blue, Green, Red, Yellow, Purple, Empty, OwnedByAnimation };

struct Sprite {
  Sprite(SpriteID id) : id_(id) {}
  Sprite(SpriteID id, SDL_Texture *sprite, SDL_Texture *selected)
      : id_(id), sprite_(sprite), selected_(selected) {}
  Sprite(const Sprite &s)
      : id_(s.id_), sprite_(s.sprite_), selected_(s.selected_) {}

  ~Sprite() {
    if (sprite_) {
      SDL_DestroyTexture(sprite_);
    }
    if (selected_) {
      SDL_DestroyTexture(selected_);
    }
  }
  auto operator()() const { return sprite_; }
  SpriteID id_;
  SDL_Texture *sprite_ = nullptr;
  SDL_Texture *selected_ = nullptr;
};

class AssetManagerInterface {
 public:
  virtual ~AssetManagerInterface() {}
  virtual SDL_Texture *GetBackgroundTexture() = 0;
  virtual std::shared_ptr<Sprite> GetSprite(SpriteID id) = 0;
  virtual SDL_Texture *GetSpriteAsTexture(SpriteID id) = 0;
  virtual TTF_Font *GetFont(int id) = 0;
};

class AssetManager : public AssetManagerInterface {
 public:
  AssetManager(SDL_Renderer *renderer);
  AssetManager(const AssetManager &) = delete;
  virtual ~AssetManager() noexcept;

  virtual SDL_Texture *GetBackgroundTexture() override { return background_texture_; }
  virtual std::shared_ptr<Sprite> GetSprite(SpriteID id) override { return sprites_[id]; }
  virtual SDL_Texture * GetSpriteAsTexture(SpriteID id) override { return sprites_[id]->sprite_; }
  virtual TTF_Font *GetFont(int id) override { return fonts_[id]; }

 private:
  std::vector<TTF_Font *> fonts_;
  std::vector<std::shared_ptr<Sprite>> sprites_;
  SDL_Texture *background_texture_;
};
