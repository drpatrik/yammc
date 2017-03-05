#pragma once

#include "asset_manager.h"

class Element final {
 public:
  Element() : sprite_(std::make_shared<Sprite>(SpriteID::Empty)) {}

  explicit Element(SpriteID id) : sprite_(std::make_shared<Sprite>(id)) {}

  explicit Element(const std::shared_ptr<const Sprite>& sprite) : sprite_(sprite) {}

  Element(const Element& e) : sprite_(e.sprite_) {}

  Element(Element&& rhs) noexcept { Swap(*this, rhs); }

  Element& operator=(Element rhs) noexcept {
    Swap(*this, rhs);

    return *this;
  }

  bool operator==(const SpriteID& id) const { return sprite_->id() == id; }

  bool operator==(const Element& e) const { return sprite_->id() == e.id(); }

  bool operator!=(const Element& e) const { return sprite_->id() != e.id(); }

  bool operator!=(const SpriteID& id) const { return sprite_->id() != id; }

  operator SpriteID() const { return sprite_->id(); }

  SpriteID id() const { return sprite_->id(); }

  bool IsEmpty() const { return sprite_->IsEmpty(); }

  bool IsSelected() const { return is_selected_; }

  void Select() { is_selected_ = true; }

  void Unselect() { is_selected_ = false; }

  void Visible(bool flag) { is_visible = flag; }

  void Render(SDL_Renderer *renderer, int x, int y, bool render_always = false) const {
    if (!render_always && (!is_visible || sprite_->IsEmpty())) {
      return;
    }
    SDL_Rect rc { x, y, kSpriteWidth, kSpriteHeight };

    if (is_selected_) {
      SDL_RenderCopy(renderer, sprite_->selected_sprite(), nullptr, &rc);
    } else {
      SDL_RenderCopy(renderer, sprite_->sprite(), nullptr, &rc);
    }
  }

 protected:
  void Swap(Element& e1, Element& e2) {
    std::swap(e1.sprite_, e2.sprite_);
  }

 private:
  std::shared_ptr<const Sprite> sprite_;
  bool is_selected_ = false;
  bool is_visible = true;
};
