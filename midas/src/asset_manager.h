#pragma once

#include <constants.h>

#include <string>
#include <vector>
#include <random>

#include <SDL.h>
#include <SDL_TTF.h>

enum SpriteID { Blue, Green, Red, Yellow, Purple, Empty, OwnedByAnimation };

class Sprite {
 public:
  explicit Sprite(SpriteID id) : id_(id) {}

  Sprite(SpriteID id, SDL_Texture *sprite, SDL_Texture *selected) : id_(id), sprite_(sprite), selected_(selected) {
    Uint32 format;
    int access;

    SDL_QueryTexture(sprite, &format, &access, &width_, &height_);
  }

  Sprite(const Sprite& s) : id_(s.id_), sprite_(s.sprite_), width_(s.width_), height_(s.height_), selected_(s.selected_) {}

  ~Sprite() {
    if (sprite_) {
      SDL_DestroyTexture(sprite_);
      sprite_ = nullptr;
    }
    if (selected_) {
      SDL_DestroyTexture(selected_);
      selected_ = nullptr;
    }
  }

  auto id() const { return id_; }

  auto operator()() const { return sprite_; }

  auto sprite() const { return sprite_; }

  auto selected_sprite() const { return selected_; }

  int width() const { return width_; }

  int height() const { return height_; }

  bool IsEmpty() const { return (id_ == SpriteID::Empty || id_ == SpriteID::OwnedByAnimation); }

 private:
  const SpriteID id_;
  SDL_Texture *sprite_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  SDL_Texture *selected_ = nullptr;
};

enum Font { Normal, Bold };

class AssetManagerInterface {
 public:
  virtual ~AssetManagerInterface() {}
  virtual SDL_Texture *GetBackgroundTexture() = 0;
  virtual std::shared_ptr<Sprite> GetSprite(int col) = 0;
  virtual std::shared_ptr<Sprite> GetSprite() = 0;
  virtual std::shared_ptr<Sprite> GetSprite(SpriteID id) = 0;
  virtual std::vector<SDL_Texture *> GetStarTextures() = 0;
  virtual SDL_Texture *GetSpriteAsTexture(SpriteID id) = 0;
  virtual TTF_Font *GetFont(int id) = 0;
  virtual void ResetPreviousIds() = 0;
};

class AssetManager : public AssetManagerInterface {
 public:
  explicit AssetManager(SDL_Renderer *renderer);

  AssetManager(const AssetManager&) = delete;

  virtual ~AssetManager() noexcept;

  virtual SDL_Texture *GetBackgroundTexture() override { return background_texture_; }

  virtual std::shared_ptr<Sprite> GetSprite() override { return sprites_.at(distribution_(engine_)); }

  virtual std::shared_ptr<Sprite> GetSprite(int col) override {
    int id;

    do {
      id = distribution_(engine_);
    } while (previous_ids_.at(col) == id || (col > 0 && previous_ids_.at(col - 1) == id));
    previous_ids_[col] = static_cast<SpriteID>(id);
    return sprites_.at(id);
  }

  virtual std::shared_ptr<Sprite> GetSprite(SpriteID id) override {
    if (id > SpriteID::Empty) {
      return sprites_[SpriteID::Empty];
    }
    return sprites_.at(id);
  }

  virtual std::vector<SDL_Texture *> GetStarTextures() override {
    return star_textures_;
  }

  virtual SDL_Texture * GetSpriteAsTexture(SpriteID id) override { return (*GetSprite(id))(); }

  virtual TTF_Font *GetFont(int id) override { return fonts_[id]; }

  virtual void ResetPreviousIds() override {
    previous_ids_ = std::vector<SpriteID>(kCols, SpriteID::Empty);
  }

 private:
  std::vector<SpriteID> previous_ids_ = std::vector<SpriteID>(kCols, SpriteID::Empty);
  std::vector<TTF_Font *> fonts_;
  std::vector<std::shared_ptr<Sprite>> sprites_;
  std::vector<SDL_Texture *> star_textures_;
  SDL_Texture *background_texture_;
  std::mt19937 engine_ {std::random_device{}()};
  std::uniform_int_distribution<int> distribution_{ 0, kNumSprites - 1 };
};
