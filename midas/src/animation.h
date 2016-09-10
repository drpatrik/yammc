#pragma once

#include "element.h"
#include "grid.h"
#include <SDL2/SDL.h>

class Animation {
public:
  explicit Animation(SDL_Renderer *renderer)
      : renderer_(renderer), row_(-1), col_(-1) {}

  Animation(SDL_Renderer *renderer, int row, int col)
      : renderer_(renderer), row_(row), col_(col) {}

  virtual ~Animation() noexcept = default;

  virtual void Start() {}

  virtual void Update() {}

  virtual bool IsDone() { return true; }

  virtual int row() const { return row_; }

  virtual int col() const { return col_; }

  operator SDL_Renderer *() {
    assert(renderer_);
    return renderer_;
  }

private:
  SDL_Renderer *renderer_;
  int row_;
  int col_;
};

class SwitchAnimation : public Animation {
public:
  SwitchAnimation(SDL_Renderer *renderer, int row, int col, Grid &grid,
                  const Position &p1, const Position &p2, bool has_match,
                  const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, row, col), grid_(grid), p1_(p1), p2_(p2),
        has_match_(has_match), asset_manager_(asset_manager) {}

  virtual void Start() override {
    grid_.At(p1_).Unselect();
    grid_.At(p2_).Unselect();

    on_same_row = (p1_.first == p2_.first);

    if (on_same_row) {
      if (p2_.second > p1_.second) {
        std::swap(p1_, p2_);
      }
    } else if (p2_.first > p1_.first) {
      std::swap(p1_, p2_);
    }
    std::swap(id1_, grid_.At(p1_));
    rc1_ = {col_to_pixel(p1_.second), row_to_pixel(p1_.first), kSpriteWidth,
            kSpriteWidth};
    std::swap(id2_, grid_.At(p2_));
    rc2_ = {col_to_pixel(p2_.second), row_to_pixel(p2_.first), kSpriteWidth,
            kSpriteHeight};
  }

  virtual void Update() override { ticks_++; }

  virtual bool IsDone() override {
    if (ticks_ <= ((has_match_) ? 8 : 16)) {
      int sign1 = (ticks_ <= 8) ? 1 : -1;
      int sign2 = (ticks_ <= 8) ? -1 : 1;

      if (on_same_row) {
        rc1_.x += -(5 * sign1);
        rc2_.x += -(5 * sign2);
      } else {
        rc1_.y += -(5 * sign1);
        rc2_.y += -(5 * sign2);
      }
      SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id1_), nullptr,
                     &rc1_);
      SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id2_), nullptr,
                     &rc2_);
      return false;
    }
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id1_), nullptr,
                   &rc1_);
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id2_), nullptr,
                   &rc2_);

    if (has_match_) {
      std::swap(id1_, id2_);
    }
    std::swap(id1_, grid_.At(p1_));
    std::swap(id2_, grid_.At(p2_));

    return true;
  }

private:
  Grid &grid_;
  Position p1_;
  Position p2_;
  bool has_match_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Rect rc1_;
  SDL_Rect rc2_;
  int ticks_ = 0;
  Element id1_ = Element(OwnedByAnimation);
  Element id2_ = Element(OwnedByAnimation);
  bool on_same_row = false;
};

class MatchAnimation : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid &grid,
                 const std::set<Position> &matches,
                 const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer), grid_(grid), matches_(matches),
        asset_manager_(asset_manager) {
    scale_rc_ = {0, 0, kSpriteWidth, kSpriteHeight};
  }

  virtual void Start() override {
    size_t i = 0;

    ids_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (auto &m : matches_) {
      std::swap(ids_[i], grid_.At(m));
      i++;
    }
  }

  virtual void Update() override {
    size_t i = 0;

    for (auto &m : matches_) {
      int x = col_to_pixel(m.second) + scale_rc_.x;
      int y = row_to_pixel(m.first) + scale_rc_.y;

      SDL_Rect rc = {x, y, scale_rc_.w, scale_rc_.h};
      SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(ids_[i]),
                     nullptr, &rc);
      i++;
    }
    scale_rc_.x += 2;
    scale_rc_.y += 2;
    scale_rc_.w -= 4;
    scale_rc_.h -= 4;
  }

  virtual bool IsDone() override {
    if (scale_rc_.w <= 0 || scale_rc_.h <= 0) {
      for (auto &m : matches_) {
        grid_.At(m) = Element(SpriteID::Empty);
      }
      return true;
    }
    return false;
  }

private:
  SDL_Rect scale_rc_;
  Grid &grid_;
  std::set<Position> matches_;
  std::vector<Element> ids_;
  const std::shared_ptr<AssetManager> asset_manager_;
};

class MoveDownAnimation : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer), grid_(grid), p_(p), asset_manager_(asset_manager) {
  }

  virtual void Start() override {
    std::swap(id_, grid_.At(p_));
    rc_ = {col_to_pixel(p_.second), row_to_pixel(p_.first) - kSpriteHeight,
           kSpriteWidth, kSpriteHeight};
    y_ = rc_.y;
    end_pos_ = y_ + kSpriteHeight;
  }

  virtual void Update() override {
    rc_.y = static_cast<int>(y_);
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id_), nullptr,
                   &rc_);
    y_ += (10.0 * (static_cast<double>(kSpriteHeight) / kFPS));
  }

  virtual bool IsDone() override {
    if (y_ <= end_pos_) {
      return false;
    }
    std::swap(id_, grid_.At(p_));
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(grid_.At(p_)),
                   nullptr, &rc_);
    return true;
  }

private:
  Grid &grid_;
  Position p_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Rect rc_;
  double end_pos_ = 0.0;
  double y_ = 0.0;
  Element id_ = Element(OwnedByAnimation);
};
