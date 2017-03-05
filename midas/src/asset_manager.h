#pragma once

#include "constants.h"
#include "audio.h"

#include <string>
#include <vector>
#include <random>

#include <SDL.h>
#include <SDL_TTF.h>

enum SpriteID { Blue, Green, Red, Yellow, Purple, Empty, OwnedByAnimation };

class Sprite final {
 public:
  explicit Sprite(SpriteID id) : id_(id) {}

  Sprite(SpriteID id, SDL_Texture *sprite, SDL_Texture *selected) : id_(id), sprite_(sprite), selected_(selected) {
    Uint32 format;
    int access;

    SDL_QueryTexture(sprite, &format, &access, &width_, &height_);
  }

  Sprite(const Sprite& s) : id_(s.id_), sprite_(s.sprite_), width_(s.width_), height_(s.height_), selected_(s.selected_) {}

  auto id() const { return id_; }

  SDL_Texture* operator()() const { return sprite_; }

  SDL_Texture* sprite() const { return sprite_; }

  SDL_Texture* selected_sprite() const { return selected_; }

  int width() const { return width_; }

  int height() const { return height_; }

  bool IsEmpty() const { return (id_ == SpriteID::Empty || id_ == SpriteID::OwnedByAnimation); }

 private:
  const SpriteID id_;
  SDL_Texture* sprite_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  SDL_Texture* selected_ = nullptr;
};

enum Font { Normal, Bold, Small };

class AssetManagerInterface {
 public:
  virtual ~AssetManagerInterface() {}
  virtual SDL_Texture *GetBackgroundTexture() const = 0;
  virtual std::shared_ptr<const Sprite> GetSprite(int col) const = 0;
  virtual std::shared_ptr<const Sprite> GetSprite() const = 0;
  virtual std::shared_ptr<const Sprite> GetSprite(SpriteID id) const = 0;
  virtual std::vector<SDL_Texture*> GetStarTextures() const = 0;
  virtual std::vector<SDL_Texture*> GetExplosionTextures() const= 0;
  virtual SDL_Texture *GetSpriteAsTexture(SpriteID id) const= 0;
  virtual TTF_Font *GetFont(int id) const = 0;
  virtual void ResetPreviousIds() = 0;
};

class AssetManager final : public AssetManagerInterface {
 public:
  explicit AssetManager(SDL_Renderer *renderer);

  AssetManager(const AssetManager&) = delete;

  virtual ~AssetManager() noexcept;

  virtual SDL_Texture *GetBackgroundTexture() const override { return background_texture_; }

  virtual std::shared_ptr<const Sprite> GetSprite() const override { return sprites_.at(distribution_(engine_)); }

  virtual std::shared_ptr<const Sprite> GetSprite(int col) const override {
    SpriteID id;

    do {
      id = static_cast<SpriteID>(distribution_(engine_));
    } while (previous_ids_.at(col) == id || (col < kCols - 1 && id == previous_ids_.at(col + 1)));
    previous_ids_[col] = id;
    return sprites_.at(id);
  }

  virtual std::shared_ptr<const Sprite> GetSprite(SpriteID id) const override {
    if (id > SpriteID::Empty) {
      return sprites_[SpriteID::Empty];
    }
    return sprites_.at(id);
  }

  virtual std::vector<SDL_Texture*> GetStarTextures() const override {
    return star_textures_;
  }

  virtual std::vector<SDL_Texture*> GetExplosionTextures() const override {
    return explosion_texture_;
  }

  virtual SDL_Texture * GetSpriteAsTexture(SpriteID id) const override { return (*GetSprite(id))(); }

  virtual TTF_Font *GetFont(int id) const override { return fonts_[id]; }

  virtual void ResetPreviousIds() override {
    previous_ids_ = std::vector<SpriteID>(kCols, SpriteID::Empty);
  }

  virtual const Audio& GetAudio() const { return audio_; }

 private:
  std::vector<TTF_Font *> fonts_;
  std::vector<std::shared_ptr<const Sprite>> sprites_;
  std::vector<SDL_Texture*> star_textures_;
  std::vector<SDL_Texture*> explosion_texture_;
  SDL_Texture* background_texture_;
  Audio audio_;
  mutable std::vector<SpriteID> previous_ids_ = std::vector<SpriteID>(kCols, SpriteID::Empty);
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 0, kNumSprites - 1 };
};
