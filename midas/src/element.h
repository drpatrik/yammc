#pragma once

#include <iostream>
#include "asset_manager.h"
#include "coordinates.h"

class Element {
 public:
  Element(SpriteID id) {
    sprite_ = std::make_shared<Sprite>(id);
  }
  Element(std::shared_ptr<Sprite> sprite) : sprite_(sprite) {}

  Element(const Element &e) : sprite_(e.sprite_) {}

  Element(Element &&rhs) noexcept : sprite_(std::move(rhs.sprite_)) {}

  Element& operator=(const Element &rhs) {
    Element tmp(rhs);

    std::swap(*this, tmp);

    return *this;
  }

  Element& operator=(Element &&rhs) noexcept {
    sprite_ = std::move(rhs.sprite_);

    return *this;
  }

  bool operator==(const SpriteID& id) const {
    return sprite_->id() == id;
  }

  bool operator==(const Element& e) const {
    return sprite_->id() == e.id();
  }

  bool operator!=(const Element& e) const {
    return sprite_->id() != e.id();
  }

  bool operator!=(const SpriteID& id) const {
    return sprite_->id() != id;
  }

  operator SpriteID() { return sprite_->id(); }

  SpriteID id() const { return sprite_->id(); }

  bool IsEmpty() const { return sprite_->IsEmpty(); }

  bool IsSelected() const { return is_selected_; }

  void Select() { is_selected_ = true; }

  void Unselect() { is_selected_ = false; }

  void Visible(bool flag) { is_visible = flag; }

  void Render(SDL_Renderer *renderer, int x, int y, bool render_always = false) {
    if (!render_always && (!is_visible || sprite_->IsEmpty())) {
      return;
    }
    SDL_Rect rc{x, y, 35, 35};

    if (is_selected_) {
      SDL_RenderCopy(renderer, sprite_->selected_sprite(), nullptr, &rc);
    } else {
      SDL_RenderCopy(renderer, sprite_->sprite(), nullptr, &rc);
    }
  }

 private:
  std::shared_ptr<Sprite> sprite_;
  bool is_selected_ = false;
  bool is_visible = true;
};
