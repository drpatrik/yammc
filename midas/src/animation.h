#pragma once

#include "grid.h"

class Animation {
 public:
  Animation(SDL_Renderer *renderer, Grid& grid, const std::shared_ptr<AssetManager>& asset_manager) : renderer_(renderer), grid_(grid), asset_manager_(asset_manager) {}

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
  Grid& grid_;
  std::shared_ptr<AssetManager> asset_manager_;
};

class SwitchAnimation : public Animation {
 public:
  SwitchAnimation(SDL_Renderer *renderer, Grid& grid,
                  const Position& p1, const Position& p2, bool has_match,
                  const std::shared_ptr<AssetManager>& asset_manager)
      : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2),
        has_match_(has_match) {}

  virtual void Start() override {
    GetGrid().At(p1_).Unselect();
    GetGrid().At(p2_).Unselect();

    if (p1_.row() == p2_.row()) {
      if (p2_.col() > p1_.col()) {
        std::swap(p1_, p2_);
      }
    } else if (p2_.row() > p1_.row()) {
      std::swap(p1_, p2_);
    }
    std::swap(id1_, GetGrid().At(p1_));
    rc1_ = { p1_.x(), p1_.y(), kSpriteWidth, kSpriteWidth };
    std::swap(id2_, GetGrid().At(p2_));
    rc2_ = { p2_.x(), p2_.y(), kSpriteWidth, kSpriteHeight };
  }

  virtual void Update(double = 0.0) override {
    const int kVelocity = 5;
    int sign = (ticks_ <= 8) ? 1 : -1;

    if (p1_.row() == p2_.row()) {
      rc1_.x += -(kVelocity * sign);
      rc2_.x += -(kVelocity * -sign);
    } else {
      rc1_.y += -(kVelocity * sign);
      rc2_.y += -(kVelocity * -sign);
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
  SDL_Rect rc1_;
  Element id1_ = Element(OwnedByAnimation);
  Position p2_;
  SDL_Rect rc2_;
  Element id2_ = Element(OwnedByAnimation);
  bool has_match_;
  int ticks_ = 0;
};

class MatchAnimation : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid& grid,
                 const std::set<Position>& matches,
                 const std::shared_ptr<AssetManager>& asset_manager)
      : Animation(renderer, grid, asset_manager), matches_(matches) {}

  virtual void Start() override {
    size_t i = 0;

    ids_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (const auto& m : matches_) {
      std::swap(ids_[i], GetGrid().At(m));
      i++;
    }
  }

  virtual void Update(double = 0.0) override {
    size_t i = 0;

    for (const auto& m : matches_) {
      int x = m.x() + scale_rc_.x;
      int y = m.y() + scale_rc_.y;

      SDL_Rect rc = { x, y, scale_rc_.w, scale_rc_.h };
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
      for (const auto& m : matches_) {
        GetGrid().At(m) = Element(SpriteID::Empty);
      }
      return true;
    }
    return false;
  }

private:
  std::set<Position> matches_;
  std::vector<Element> ids_;
  SDL_Rect scale_rc_ = { 0, 0, kSpriteWidth, kSpriteHeight };
};

class MoveDownAnimation : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid& grid, const Position& p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p_(p) {}

  virtual void Start() override {
    std::swap(id_, GetGrid().At(p_));
    rc_ = { p_.x(), p_.y() - kSpriteHeight, kSpriteWidth, kSpriteHeight };
    y_ = rc_.y;
    end_pos_ = y_ + kSpriteHeight;
  }

  virtual void Update(double = 0.0) override {
    const int velocity = GetGrid().IsFilling() ? 15 : 5;

    rc_.y = static_cast<int>(y_);
    RenderCopy(id_, rc_);
    y_ += (velocity * (static_cast<double>(kSpriteHeight) / kFPS));
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
