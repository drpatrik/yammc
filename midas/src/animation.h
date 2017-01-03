#pragma once

#include "grid.h"

#include <cassert>

class Animation {
 public:
  const double kTimeResolution = static_cast<double>(1.0 / kFPS);

  Animation(SDL_Renderer *renderer, Grid& grid, const std::shared_ptr<AssetManager>& asset_manager) : renderer_(renderer), grid_(grid), asset_manager_(asset_manager) {}

  virtual ~Animation() noexcept = default;

  virtual void Start() = 0;

  virtual void Update(double = 0.0) = 0;

  virtual bool IsReady() = 0;

  virtual bool RemoveIfAsked() const { return false; }

  operator SDL_Renderer *() const { return renderer_; }

  Grid& GetGrid() { return grid_; }

  void RenderCopy(SpriteID id, const SDL_Rect& rc) {
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id), nullptr, &rc);
  }

  void RenderCopy(SDL_Texture *texture, const SDL_Rect& rc) {
    SDL_RenderCopy(*this, texture, nullptr, &rc);
  }

 protected:
  double x_ = 0.0;
  double y_ = 0.0;

 private:
  SDL_Renderer *renderer_;
  Grid& grid_;
  std::shared_ptr<AssetManager> asset_manager_;
};

class SwapAnimation : public Animation {
 public:
  SwapAnimation(SDL_Renderer *renderer, Grid& grid,
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
    std::swap(element1_, GetGrid().At(p1_));
    rc1_ = { p1_.x(), p1_.y(), kSpriteWidth, kSpriteWidth };
    std::swap(element2_, GetGrid().At(p2_));
    rc2_ = { p2_.x(), p2_.y(), kSpriteWidth, kSpriteHeight };

    if (p1_.row() == p2_.row()) {
      t1_ = &rc1_.x;
      t2_ = &rc2_.x;
      x_ = rc1_.x;
      y_ = rc2_.x;
    } else {
      t1_ = &rc1_.y;
      t2_ = &rc2_.y;
      x_ = rc1_.y;
      y_ = rc2_.y;
    }
  }

  virtual void Update(double delta) override {
    const double sign = (pixels_moved_ < kSpriteWidth) ? 1.0 : -1.0;
    const double kVelocity = delta * 300;

    x_ += (kVelocity * -sign);
    *t1_ = static_cast<int>(x_);

    RenderCopy(element1_, rc1_);

    y_ += (kVelocity * sign);
    *t2_ = static_cast<int>(y_);

    RenderCopy(element2_, rc2_);

    pixels_moved_ += kVelocity;
  }

  virtual bool IsReady() override {
    if (pixels_moved_ < ((has_match_) ? kSpriteWidth : kSpriteWidth * 2.0)) {
      return false;
    }
    RenderCopy(element1_, rc1_);
    RenderCopy(element2_, rc2_);
    if (has_match_) {
      std::swap(element1_, element2_);
    }
    std::swap(element1_, GetGrid().At(p1_));
    std::swap(element2_, GetGrid().At(p2_));

    return true;
  }

private:
  int *t1_ = nullptr;
  int *t2_ = nullptr;
  double pixels_moved_ = 0.0;
  Position p1_;
  SDL_Rect rc1_;
  Element element1_ = Element(OwnedByAnimation);
  Position p2_;
  SDL_Rect rc2_;
  Element element2_ = Element(OwnedByAnimation);
  bool has_match_;
};

class MatchAnimation : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid& grid,
                 const std::set<Position>& matches,
                 const std::shared_ptr<AssetManager>& asset_manager)
      : Animation(renderer, grid, asset_manager), matches_(matches) {}

  virtual void Start() override {
    size_t i = 0;

    elements_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (const auto& m : matches_) {
      std::swap(elements_[i], GetGrid().At(m));
      i++;
    }
  }

  virtual void Update(double delta) override {
    size_t i = 0;

    for (const auto& m : matches_) {
      int x = static_cast<int>(m.x() + x_);
      int y = static_cast<int>(m.y() + y_);

      SDL_Rect rc = { x, y, static_cast<int>(scale_w_), static_cast<int>(scale_h_) };
      RenderCopy(elements_[i], rc);
      i++;
    }
    x_ += (100 * delta);
    y_ += (100 * delta);
    scale_w_ -= (200 * delta);
    scale_h_ -= (200 * delta);
  }

  virtual bool IsReady() override {
    if (scale_w_ <= 0.0 || scale_h_ <= 0.0) {
      for (const auto& m : matches_) {
        GetGrid().At(m) = Element(SpriteID::Empty);
      }
      return true;
    }
    return false;
  }

private:
  std::set<Position> matches_;
  std::vector<Element> elements_;
  double scale_w_ = kSpriteWidth;
  double scale_h_ = kSpriteHeight;
};

class MoveDownAnimation : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid& grid, const Position& p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p_(p) {}

  virtual void Start() override {
    std::swap(element_, GetGrid().At(p_));
    rc_ = { p_.x(), p_.y() - kSpriteHeight, kSpriteWidth, kSpriteHeight };
    y_ = rc_.y;
    end_pos_ = y_ + kSpriteHeight;
  }

  virtual void Update(double delta) override {
    const double kIncY = GetGrid().IsFilling() ? delta * 500 : delta * 300;

    rc_.y = static_cast<int>(y_);
    RenderCopy(element_, rc_);
    y_ += kIncY;
  }

  virtual bool IsReady() override {
    if (y_ <= end_pos_) {
      return false;
    }
    std::swap(element_, GetGrid().At(p_));
    RenderCopy(GetGrid().At(p_), rc_);
    return true;
  }

private:
  Position p_;
  SDL_Rect rc_;
  double end_pos_ = 0.0;
  Element element_ = Element(OwnedByAnimation);
};

class WiggleAnimation : public Animation {
public:
  WiggleAnimation(SDL_Renderer *renderer, Grid &grid, const Position& p1, const Position& p2, std::shared_ptr<AssetManager> &asset_manager) : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2) {}

  ~WiggleAnimation() {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
  }

  virtual void Start() override {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
  }

  virtual void Update(double delta) override {
    int offset_x,offset_y;
    std::tie(offset_x, offset_y) = frames_[frame_];

    e1_.Render(*this, p1_.x() + offset_x, p1_.y() + offset_y, true);
    e2_.Render(*this, p2_.x() + offset_x, p2_.y() + offset_y, true);

    ticks_ += delta;
    animation_ticks_ += delta;
    if (animation_ticks_ >= (kTimeResolution * 2.0)) {
      frame_ = (++frame_ % frames_.size());
      animation_ticks_ = 0.0;
    }
  }

  virtual bool IsReady() override {
    if (ticks_ >= 1.0) {
      return true;
    }
    return false;
  }

  virtual bool RemoveIfAsked() const override { return true; }

 private:
  Position p1_;
  Position p2_;
  Element e1_ = Element(SpriteID::OwnedByAnimation);
  Element e2_ = Element(SpriteID::OwnedByAnimation);
  size_t frame_ = 0;
  double ticks_ = 0.0;
  double animation_ticks_ = 0.0;
  const std::vector<std::pair<int, int>> frames_ = {
    std::make_pair(3, 0),
    std::make_pair(3, 3),
    std::make_pair(0, 3),
    std::make_pair(-3, 0),
    std::make_pair(-3, -3),
    std::make_pair(0, -3),
  };
};

class CountDownAnimation : public Animation {
public:
  CountDownAnimation(SDL_Renderer *renderer, Grid &grid, std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), star_textures_(asset_manager->GetStarTextures()) {
    assert(coordinates_.size() == kGameTime);
  }

  virtual void Start() override {}

  virtual void Update(double delta) override {
    int x, y;
    std::tie(x, y) = coordinates_[timer_];

    RenderCopy(star_textures_.at(frame_), {x - 15, y - 15, 30, 30 });

    animation_ticks_ += delta;
    if (animation_ticks_ > kTimeResolution) {
      frame_ = (++frame_ % star_textures_.size());
      animation_ticks_ = 0.0;
    }
    movement_ticks_ += delta;
    if (movement_ticks_ > 1.0) {
      timer_++;
      movement_ticks_ = 0.0;
    }
  }

  virtual bool IsReady() override { return timer_ == coordinates_.size(); }

  int GetTimeLeft() const { return kGameTime - timer_; }

 private:
  int frame_ = 0;
  double animation_ticks_ = 0.0;
  double movement_ticks_ = 0.0;
  size_t timer_ = 0;
  std::vector<SDL_Texture*> star_textures_;
  const std::vector<std::pair<int, int>> coordinates_ = {
    std::make_pair(262, 555),
    std::make_pair(258, 552),
    std::make_pair(256, 548),
    std::make_pair(253, 545),
    std::make_pair(252, 540),
    std::make_pair(254, 535),
    std::make_pair(251, 530),
    std::make_pair(253, 525),
    std::make_pair(251, 522),
    std::make_pair(254, 520),
    std::make_pair(250, 518),
    std::make_pair(245, 513),
    std::make_pair(240, 513),
    std::make_pair(235, 514),
    std::make_pair(230, 511),
    std::make_pair(228, 509),
    std::make_pair(225, 511),
    std::make_pair(220, 512),
    std::make_pair(217, 510),
    std::make_pair(218, 508),
    std::make_pair(216, 507),
    std::make_pair(215, 506),
    std::make_pair(213, 505),
    std::make_pair(212, 504),
    std::make_pair(210, 503),
    std::make_pair(212, 502),
    std::make_pair(214, 500),
    std::make_pair(210, 495),
    std::make_pair(212, 490),
    std::make_pair(211, 485),
    std::make_pair(212, 480),
    std::make_pair(210, 475),
    std::make_pair(212, 470),
    std::make_pair(210, 465),
    std::make_pair(211, 460),
    std::make_pair(212, 455),
    std::make_pair(213, 450),
    std::make_pair(210, 445),
    std::make_pair(211, 440),
    std::make_pair(212, 435),
    std::make_pair(210, 430),
    std::make_pair(208, 425),
    std::make_pair(210, 420),
    std::make_pair(208, 415),
    std::make_pair(210, 410),
    std::make_pair(208, 405),
    std::make_pair(210, 400),
    std::make_pair(208, 395),
    std::make_pair(210, 390),
    std::make_pair(210, 385),
    std::make_pair(209, 380),
    std::make_pair(207, 375),
    std::make_pair(200, 372),
    std::make_pair(196, 372),
    std::make_pair(192, 372),
    std::make_pair(188, 372),
    std::make_pair(185, 372),
    std::make_pair(183, 372),
    std::make_pair(181, 372),
    std::make_pair(179, 372),
  };
};

class ExplosionAnimation : public Animation {
public:
  ExplosionAnimation(SDL_Renderer *renderer, Grid &grid, std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), explosion_texture_(asset_manager->GetExplosionTextures()) {}

  virtual void Start() override {}

  virtual void Update(double delta) override {
    const SDL_Rect rc { 100, 278, 71, 100 };

    RenderCopy(explosion_texture_.at(frame_), rc);
    animation_ticks_ += delta;
    if (animation_ticks_ > (kTimeResolution * 5)) {
      frame_++;
      animation_ticks_ = 0.0;
    }
  }

  virtual bool IsReady() override { return (static_cast<size_t>(frame_) >= explosion_texture_.size()); }

 private:
  int frame_ = 0;
  double animation_ticks_ = 0.0;
  std::vector<SDL_Texture*> explosion_texture_;
};
