#pragma once

#include <SDL2/SDL.h>
#include "element.h"
#include "grid.h"

class Animation {
 public:
  Animation() : row_(-1), col_(-1) {}
  Animation(int row, int col) : row_(row), col_(col) {}
  virtual ~Animation() noexcept = default;
  virtual bool Queue() const { return false; };
  virtual void Start(SDL_Renderer *) {}
  virtual void Update() {}
  virtual bool End() { return true; }
  virtual int row() const { return row_; }
  virtual int col() const { return col_; }

 private:
  int row_;
  int col_;
};

class VoidAnimation : public Animation {
};

class SwitchAnimation : public Animation {
 public:
  SwitchAnimation(int row, int col, Grid &grid, const Position &p1, Position &p2, bool has_match,  const std::shared_ptr<AssetManager>& asset_manager) : Animation(row, col), grid_(grid), p1_(p1), p2_(p2), has_match_(has_match), asset_manager_(asset_manager) {}

  virtual bool Queue() const override { return true; }

  virtual void Start(SDL_Renderer *renderer) override {
    renderer_ = renderer;

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
    rc1_ = {col_to_pixel(p1_.second), row_to_pixel(p1_.first), 35, 35};
    std::swap(id2_, grid_.At(p2_));
    rc2_ = {col_to_pixel(p2_.second), row_to_pixel(p2_.first), 35, 35};
  }

  virtual void Update() override {
    ticks_++;
  }
  virtual bool End() override {
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
      SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(id1_), nullptr, &rc1_);
      SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(id2_), nullptr, &rc2_);
      return false;
    }
    SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(id1_), nullptr, &rc1_);
    SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(id2_), nullptr, &rc2_);

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
  SDL_Renderer *renderer_ = nullptr;
};

class MatchAnimation : public Animation {
 public:
  MatchAnimation(int row, int col, const std::set<Position>& matches) : Animation(row, col), matches_(matches) {
  }

  virtual bool Queue() const override { return true; }

  virtual void Start(SDL_Renderer *renderer) override {
    renderer_ = renderer;
  }

  virtual void Update() override {
    ticks_++;
    if (++blink_ < 5) {
      for (auto& m:matches_) {
        int row, col;

        std::tie(row, col) = m;
        SDL_Rect rc{col_to_pixel(col), row_to_pixel(row),35,35};

        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); // White
        SDL_RenderFillRect(renderer_,&rc);
      }
    }
    if (blink_ > 10) {
      blink_ = 0;
    }
  }

  virtual bool End() override {
    if (ticks_ > 30) {
      return true;
    }
    return false;
  }

 private:
  int blink_ = 0;
  int ticks_ = 0;
  std::set<Position> matches_;
  SDL_Renderer *renderer_ = nullptr;
};

class MoveDownAnimation : public Animation {
 public:
  MoveDownAnimation(int row, int col, Grid &grid, const Position &p, const std::shared_ptr<AssetManager>& asset_manager) : Animation(row, col), grid_(grid), p_(p), asset_manager_(asset_manager) {}

  virtual bool Queue() const override { return true; }

  virtual void Start(SDL_Renderer *renderer) override {
    renderer_ = renderer;
    std::swap(id_,grid_.At(p_));
    rc_ = {col_to_pixel(p_.second), row_to_pixel(p_.first) - 35, 35, 35};
  }

  virtual void Update() override {
    ticks_++;
  }

  virtual bool End() override {
    if (ticks_ <= 7) {
      rc_.y += 5;
      SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(id_), nullptr, &rc_);
      return false;
    }
    std::swap(id_,grid_.At(p_));
    SDL_RenderCopy(renderer_, asset_manager_->GetSpriteAsTexture(grid_.At(p_)), nullptr, &rc_);
    return true;
  }

 private:
  Grid &grid_;
  Position p_;
  std::shared_ptr<AssetManager> asset_manager_;
  SDL_Rect rc_;
  int ticks_ = 0;
  Element id_ = Element(OwnedByAnimation);
  SDL_Renderer *renderer_ = nullptr;
};
