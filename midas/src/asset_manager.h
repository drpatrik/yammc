#pragma once

#include "constants.h"
#include "audio.h"
#include "sprite.h"

#include <string>
#include <vector>
#include <random>

#include <SDL_TTF.h>

class AssetManagerInterface {
 public:
  virtual ~AssetManagerInterface() {}

  virtual std::shared_ptr<const Sprite> GetSprite() const = 0;

  virtual std::shared_ptr<const Sprite> GetSprite(int) const = 0;

  virtual std::shared_ptr<const Sprite> GetSprite(SpriteID) const = 0;

  virtual void ResetPreviousIds() = 0;
};

enum Font { Normal, Bold, Small };

class AssetManager final : public AssetManagerInterface {
 public:
  explicit AssetManager(SDL_Renderer *renderer);

  AssetManager(const AssetManager&) = delete;

  virtual ~AssetManager() noexcept;

  virtual SDL_Texture *GetBackgroundTexture() const { return background_texture_; }

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

  virtual std::vector<SDL_Texture*> GetStarTextures() const {
    return star_textures_;
  }

  virtual std::vector<SDL_Texture*> GetExplosionTextures() const {
    return explosion_texture_;
  }

  virtual SDL_Texture * GetSpriteAsTexture(SpriteID id) const { return (*GetSprite(id))(); }

  virtual TTF_Font *GetFont(int id) const { return fonts_[id]; }

  virtual void ResetPreviousIds() override { previous_ids_ = std::vector<SpriteID>(kCols, SpriteID::Empty); }

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
