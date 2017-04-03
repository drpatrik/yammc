#pragma once

#include <utility>

#include <SDL.h>

enum SpriteID { Blue, Green, Red, Yellow, Purple, Empty, OwnedByAnimation };

class Sprite final {
 public:
  explicit Sprite(SpriteID id) : id_(id) {}

  Sprite(SpriteID id, SDL_Texture *sprite, SDL_Texture *selected_sprite) : id_(id), sprite_(sprite), selected_sprite_(selected_sprite) {
    Uint32 format;
    int access;

    SDL_QueryTexture(sprite_, &format, &access, &width_, &height_);
  }

  Sprite(const Sprite& s) : id_(s.id_), sprite_(s.sprite_), width_(s.width_), height_(s.height_), selected_sprite_(s.selected_sprite_) {}

  Sprite(Sprite&& other) {
    swap(*this, other);
  }

  Sprite& operator=(Sprite other) {
    swap(*this, other);

    return *this;
  }

  SpriteID id() const { return id_; }

  SDL_Texture* operator()() const { return sprite_; }

  SDL_Texture* sprite() const { return sprite_; }

  SDL_Texture* selected_sprite() const { return selected_sprite_; }

  int width() const { return width_; }

  int height() const { return height_; }

  bool IsEmpty() const { return (id_ == SpriteID::Empty || id_ == SpriteID::OwnedByAnimation); }

  friend void swap(Sprite& s1, Sprite& s2) {
    using std::swap;

    swap(s1.id_, s2.id_);
    swap(s1.sprite_, s2.sprite_);
    swap(s1.width_, s2.width_);
    swap(s1.height_, s2.height_);
    swap(s1.selected_sprite_, s2.selected_sprite_);
  }

 private:
  SpriteID id_;
  SDL_Texture* sprite_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  SDL_Texture* selected_sprite_ = nullptr;
};
