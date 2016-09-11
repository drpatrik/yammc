#pragma once

#include "grid.h"

class Animation {
 public:
  Animation(SDL_Renderer *renderer, Grid &grid, const std::shared_ptr<AssetManager>& asset_manager) : renderer_(renderer), grid_(grid), asset_manager_(asset_manager) {}

  virtual ~Animation() noexcept = default;

  virtual void Start() = 0;

  virtual void Update(double = 0.0) = 0;

  virtual bool IsDone() = 0;

  operator SDL_Renderer *() {
    assert(renderer_);
    return renderer_;
  }

  Grid& GetGrid() { return grid_; }

  const AssetManager& GetAsset() const { return *asset_manager_.get(); }

  void RenderCopy(SpriteID id, const SDL_Rect& rc) {
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id), nullptr, &rc);
  }

 protected:
  double x_ = 0.0;
  double y_ = 0.0;

 private:
  SDL_Renderer *renderer_;
  Grid &grid_;
  std::shared_ptr<AssetManager> asset_manager_;
};

class SwitchAnimation : public Animation {
 public:
  SwitchAnimation(SDL_Renderer *renderer, Grid &grid,
                  const Position &p1, const Position &p2, bool has_match,
                  const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2),
        has_match_(has_match) {}

  virtual void Start() override {
    GetGrid().At(p1_).Unselect();
    GetGrid().At(p2_).Unselect();

    if (p1_.first == p2_.first) {
      if (p2_.second > p1_.second) {
        std::swap(p1_, p2_);
      }
    } else if (p2_.first > p1_.first) {
      std::swap(p1_, p2_);
    }
    std::swap(id1_, GetGrid().At(p1_));
    rc1_ = {col_to_pixel(p1_.second), row_to_pixel(p1_.first), kSpriteWidth, kSpriteWidth};
    std::swap(id2_, GetGrid().At(p2_));
    rc2_ = {col_to_pixel(p2_.second), row_to_pixel(p2_.first), kSpriteWidth, kSpriteHeight};
  }

  virtual void Update(double = 0.0) override {
    int sign = (ticks_ <= 8) ? 1 : -1;

    if (p1_.first == p2_.first) {
      rc1_.x += -(5 * sign);
      rc2_.x += -(5 * -sign);
    } else {
      rc1_.y += -(5 * sign);
      rc2_.y += -(5 * -sign);
    }
    RenderCopy(id1_, rc1_);
    RenderCopy(id2_, rc2_);
    ticks_++;
  }

  virtual bool IsDone() override {
    if (ticks_ <= ((has_match_) ? 8 : 16)) {
      return false;
    }
    RenderCopy(id1_, rc1_);
    RenderCopy(id2_, rc2_);
    if (has_match_) {
      std::swap(id1_, id2_);
    }
    std::swap(id1_, GetGrid().At(p1_));
    std::swap(id2_, GetGrid().At(p2_));

    return true;
  }

private:
  Position p1_;
  Position p2_;
  SDL_Rect rc1_;
  SDL_Rect rc2_;
  bool has_match_;
  int ticks_ = 0;
  Element id1_ = Element(OwnedByAnimation);
  Element id2_ = Element(OwnedByAnimation);
};

class MatchAnimation : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid &grid,
                 const std::set<Position> &matches,
                 const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), matches_(matches) {
    scale_rc_ = {0, 0, kSpriteWidth, kSpriteHeight};
  }

  virtual void Start() override {
    size_t i = 0;

    ids_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (auto &m : matches_) {
      std::swap(ids_[i], GetGrid().At(m));
      i++;
    }
  }

  virtual void Update(double = 0.0) override {
    size_t i = 0;

    for (auto &m : matches_) {
      int x = col_to_pixel(m.second) + scale_rc_.x;
      int y = row_to_pixel(m.first) + scale_rc_.y;

      SDL_Rect rc = {x, y, scale_rc_.w, scale_rc_.h};
      RenderCopy(ids_[i], rc);
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
        GetGrid().At(m) = Element(SpriteID::Empty);
      }
      return true;
    }
    return false;
  }

private:
  SDL_Rect scale_rc_;
  std::set<Position> matches_;
  std::vector<Element> ids_;
};

class MoveDownAnimation : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p_(p) {}

  virtual void Start() override {
    std::swap(id_, GetGrid().At(p_));
    rc_ = {col_to_pixel(p_.second), row_to_pixel(p_.first) - kSpriteHeight,
           kSpriteWidth, kSpriteHeight};
    y_ = rc_.y;
    end_pos_ = y_ + kSpriteHeight;
  }

  virtual void Update(double = 0.0) override {
    rc_.y = static_cast<int>(y_);
    RenderCopy(id_, rc_);
    y_ += (8.0 * (static_cast<double>(kSpriteHeight) / kFPS));
  }

  virtual bool IsDone() override {
    if (y_ <= end_pos_) {
      return false;
    }
    std::swap(id_, GetGrid().At(p_));
    RenderCopy(GetGrid().At(p_), rc_);
    return true;
  }

private:
  Position p_;
  SDL_Rect rc_;
  double end_pos_ = 0.0;
  Element id_ = Element(OwnedByAnimation);
};
