#pragma once

#include "asset_manager.h"
#include "coordinates.h"

class Element {
 public:
  Element(AssetManager::Sprite& sprite) : sprite_(sprite) {}
  Element(AssetManager::Sprite& sprite, int row, int col) : sprite_(sprite), row_(row), col_(col) {}
  Element(const Element &e) : sprite_(e.sprite_), row_(e.row_), col_(e.col_) {}
  Element& operator=(const Element &e) {
    Element tmp(e);

    std::swap(*this, tmp);

    return *this;
  }
  bool operator==(const Element& a) const {
    return sprite_.name_ == a();
  }
  bool operator!=(const Element& a) const {
    return sprite_.name_ != a();
  }

  int row() const { return row_; }
  int col() const { return col_; }
  SpriteName& operator()() { return sprite_.name_; }
  const SpriteName& operator()() const { return sprite_.name_; }
  SpriteName name() const { return sprite_.name_; }
  bool IsEmpty() const { return sprite_.name_ == SpriteName::Empty; }
  bool IsSelected() const { return is_selected_; }
  void Selected() { is_selected_ = true; }
  void ClearSelected() { is_selected_ = false; }
  void Render(SDL_Renderer *renderer) {
    if (sprite_.name_ == SpriteName::Empty) {
      return;
    }
    SDL_Rect rc{col_to_pixel(col_), row_to_pixel(row_), 35, 35};

    if (is_selected_) {
      SDL_RenderCopy(renderer, sprite_.selected_, nullptr, &rc);
    } else {
      SDL_RenderCopy(renderer, sprite_(), nullptr, &rc);
    }
  }

  friend void SwapPosition(Element &a, Element &b);

 private:
  AssetManager::Sprite& sprite_;
  int row_ = 0;
  int col_ = 0;
  bool is_selected_ = false;
};

inline void SwapPosition(Element &a, Element &b) {
  Element tmp(a);

  b.row_ = a.row_;
  b.col_ = a.col_;

  a.row_ = tmp.row_;
  a.col_ = tmp.col_;
}
