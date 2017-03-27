#pragma once

#include "grid.h"
#include "score.h"
#include "text.h"

namespace {

// This function should really return x,y in pixels and interpolate if neccessary
const Position& FindPositionForScoreAnimation(const std::vector<Position>& c_matches, int chains) {
  if (chains == 1) {
    return c_matches[(c_matches.size() / 2)];
  }
  auto matches = c_matches;

  while (!matches.empty()) {
    auto match = matches.back();
    matches.pop_back();
    auto it = std::find(std::begin(matches), std::end(matches), match);

    if (it != std::end(matches)) {
      return *it;
    }
  }
  int chain_size = (c_matches.size() / 2);

  bool is_vertical_chain = (c_matches[0].row() == c_matches[chain_size - 1].row());

  return (is_vertical_chain) ? c_matches[(chain_size / 2)] : c_matches[(c_matches.size()) / 2];
}

}

class Animation {
public:
  const double kTimeResolution = static_cast<double>(1.0 / kFPS);

  Animation(SDL_Renderer *renderer, Grid &grid,
            const std::shared_ptr<AssetManager> &asset_manager)
      : renderer_(renderer), grid_(grid), asset_manager_(asset_manager) {}

  virtual ~Animation() noexcept = default;

  virtual void Start() = 0;

  virtual void Update(double) = 0;

  virtual bool IsReady() = 0;

  virtual bool Idle() const { return false; }

  virtual bool LockBoard() const { return true; }

  operator SDL_Renderer *() const { return renderer_; }

  Grid &GetGrid() { return grid_; }

  const AssetManager& GetAsset() const { return *asset_manager_; }

  const Audio& GetAudio() const { return asset_manager_->GetAudio(); }

  void RenderCopy(SpriteID id, const SDL_Rect &rc) {
    SDL_RenderCopy(*this, asset_manager_->GetSpriteAsTexture(id), nullptr, &rc);
  }

  void RenderCopy(SDL_Texture *texture, const SDL_Rect &rc) {
    SDL_RenderCopy(*this, texture, nullptr, &rc);
  }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

private:
  SDL_Renderer *renderer_;
  Grid &grid_;
  std::shared_ptr<AssetManager> asset_manager_;
};

class SwapAnimation final : public Animation {
public:
  SwapAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p1,
                const Position &p2, bool has_match,
                const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2), has_match_(has_match), play_sound_(!has_match) {}

  virtual void Start() override {
    GetGrid().At(p1_).Unselect();
    GetGrid().At(p2_).Unselect();

    // Ensure the animation always move from p1 to p2.
    if (p2_.row() > p1_.row() || p2_.col() > p1_.col()) {
      std::swap(p1_, p2_);
    }
    std::swap(element1_, GetGrid().At(p1_));
    rc1_ = {p1_.x(), p1_.y(), kSpriteWidth, kSpriteWidth};
    std::swap(element2_, GetGrid().At(p2_));
    rc2_ = {p2_.x(), p2_.y(), kSpriteWidth, kSpriteHeight};

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
    const double kSign = (pixels_moved_ < kSpriteWidth) ? 1.0 : -1.0;
    const double kVelocity = delta * 300;

    x_ += (kVelocity * -kSign);
    *t1_ = static_cast<int>(x_);

    RenderCopy(element1_, rc1_);

    y_ += (kVelocity * kSign);
    *t2_ = static_cast<int>(y_);

    RenderCopy(element2_, rc2_);

    pixels_moved_ += kVelocity;
  }

  virtual bool IsReady() override {
    if (pixels_moved_ < ((has_match_) ? kSpriteWidth : kSpriteWidth * 2.0)) {
      if (pixels_moved_ >= kSpriteWidth && play_sound_) {
        GetAudio().PlaySound(SoundEffect::MoveUnSuccessful);
        play_sound_ = false;
      }
      return false;
    }
    if (has_match_) {
      GetAudio().PlaySound(SoundEffect::MoveSuccessful);
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
  SDL_Rect rc1_ { 0, 0, 0, 0};
  Element element1_ = Element(OwnedByAnimation);
  Position p2_;
  SDL_Rect rc2_ { 0, 0, 0, 0};
  Element element2_ = Element(OwnedByAnimation);
  bool has_match_;
  bool play_sound_;
};

class ScoreAnimation final : public Animation {
 public:
  ScoreAnimation(SDL_Renderer *renderer, Grid &grid,
                     const std::vector<Position> &matches, int chains,
                     int score,
                     const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), score_(score), chains_(chains) {
    auto p = FindPositionForScoreAnimation(matches, chains_);

    int width, height;
    std::tie(texture_, width, height) = CreateTextureFromFramedText(*this, GetAsset().GetFont(Small), std::to_string(score_), Color::White, Color::Black);

    rc_ = { p.x() + Center(kSpriteWidth, width), p.y() + Center(kSpriteHeight, height), width, height };
    y_ = rc_.y;
    end_pos_ = y_ - kSpriteHeightTimes1_5;
  }

  virtual ~ScoreAnimation() { SDL_DestroyTexture(texture_); }

  virtual void Start() override {
    switch (chains_) {
      case 1:
        GetAudio().PlaySound(SoundEffect::RemovedOneChain);
        break;
      case 2:
        GetAudio().PlaySound(SoundEffect::RemovedTwoChains);
        break;
      default:
        GetAudio().PlaySound(SoundEffect::RemovedManyChains);
        break;
    }
  }

  virtual void Update(double delta) override {
    SDL_Rect clip_rc;
    SDL_RenderGetClipRect(*this, &clip_rc);
    SDL_RenderSetClipRect(*this, NULL);
    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_, rc_);
    y_ -= delta * 65.0;
    SDL_RenderSetClipRect(*this, &clip_rc);
  }

  virtual bool IsReady() override { return (y_ <= end_pos_); }

 private:
  int score_;
  int chains_;
  SDL_Rect rc_;
  SDL_Texture *texture_ = nullptr;
  double end_pos_;
};

class MatchAnimation final : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid &grid,
                 const std::vector<Position> &matches, int chains,
                 const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), matches_(matches.begin(), matches.end()),
        score_animation_(renderer, grid, matches, chains, GetBasicScore(matches_.size()), asset_manager) {}

  virtual void Start() override {
    int i = 0;

    elements_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (const auto &m : matches_) {
      std::swap(elements_[i], GetGrid().At(m));
      i++;
    }
  }

  virtual void Update(double delta) override {
    if (!lock_board_) {
      score_animation_.Update(delta);
      return;
    }
    x_ += (75 * delta);
    y_ += (75 * delta);
    scale_w_ -= (150 * delta);
    scale_h_ -= (150 * delta);

    int i = 0;

    for (const auto &m : matches_) {
      int x = static_cast<int>(m.x() + x_);
      int y = static_cast<int>(m.y() + y_);

      SDL_Rect rc = { x, y, static_cast<int>(scale_w_), static_cast<int>(scale_h_) };
      RenderCopy(elements_[i], rc);
      i++;
    }
  }

  virtual bool IsReady() override {
    if (!lock_board_) {
      return score_animation_.IsReady();
    } else {
      if (scale_w_ <= 0.0 || scale_h_ <= 0.0) {
        for (const auto &m : matches_) {
          GetGrid().At(m) = Element(SpriteID::Empty);
        }
        lock_board_ = false;
        score_animation_.Start();
      }
    }
    return false;
  }

  virtual bool LockBoard() const override { return lock_board_; }

private:
  std::set<Position> matches_;
  std::vector<Element> elements_;
  double scale_w_ = kSpriteWidth;
  double scale_h_ = kSpriteHeight;
  bool lock_board_ = true;
  ScoreAnimation score_animation_;
};

class MoveDownAnimation final : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p_(p) {}

  virtual void Start() override {
    std::swap(element_, GetGrid().At(p_));
    rc_ = { p_.x(), p_.y() - kSpriteHeight, kSpriteWidth, kSpriteHeight };
    y_ = rc_.y;
  }

  virtual void Update(double delta) override {
    const double kIncY = GetGrid().IsFilling() ? delta * 500 : delta * 350;

    rc_.y = static_cast<int>(y_);
    RenderCopy(element_, rc_);
    y_ += kIncY;
  }

  virtual bool IsReady() override {
    if (y_ < p_.y()) {
      return false;
    }
    std::swap(element_, GetGrid().At(p_));
    if (p_.row() + 1 >= kRows || !GetGrid().At(p_.row() + 1, p_.col()).IsEmpty()) {
      GetAudio().PlaySound(SoundEffect::DiamondLanding);
    }
    return true;
  }

private:
  Position p_;
  SDL_Rect rc_ { 0, 0, 0, 0 };
  Element element_ = Element(OwnedByAnimation);
};

class HintAnimation final : public Animation {
public:
  HintAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p1,
                const Position &p2,
                std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2) {}

  ~HintAnimation() {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
  }

  virtual void Start() override {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
    GetAudio().PlaySound(SoundEffect::Hint);
  }

  virtual void Update(double delta) override {
    const double kTwoTimesPi = 2.0 * 3.1415926535897932384626433;
    const double kRadius = kSpriteWidth / 10.0;

    angle_ += (delta * 30);
    if (angle_ > kTwoTimesPi) {
      angle_ = 0.0;
      revolutions_++;
    }
    x_ = cos(angle_) * kRadius;
    y_ = sin(angle_) * kRadius;

    e1_.Render(*this, x_ + p1_.x(), y_ + p1_.y(), true);
    e2_.Render(*this, x_ + p2_.x(), y_ + p2_.y(), true);
  }

  virtual bool IsReady() override { return (revolutions_ >= 3) ? true : false; }

  virtual bool Idle() const override { return true; }

private:
  Position p1_;
  Position p2_;
  Element e1_ = Element(SpriteID::OwnedByAnimation);
  Element e2_ = Element(SpriteID::OwnedByAnimation);
  double angle_ = 0.0;
  int revolutions_ = 0;
};

class TimerAnimation final : public Animation {
public:
  TimerAnimation(SDL_Renderer *renderer, Grid &grid,
                 std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), star_textures_(asset_manager->GetStarTextures()) {}

  virtual void Start() override {}

  virtual void Update(double delta) override {
    const size_t kTimerStep = static_cast<size_t>(double(kGameTime) / coordinates_.size());
    int x, y;
    std::tie(x, y) = coordinates_[step_];

    RenderCopy(star_textures_.at(frame_), { x - 15, y - 15, 30, 30 });

    animation_ticks_ += delta;
    if (animation_ticks_ >= kTimeResolution) {
      frame_ = (++frame_ % star_textures_.size());
      animation_ticks_ = 0.0;
    }
    movement_ticks_ += delta;
    if (movement_ticks_ > 1.0) {
      timer_++;
      if (timer_ % kTimerStep == 0) {
        step_++;
      }
      movement_ticks_ = 0.0;
    }
    if (ShouldPlayHurryUp()) {
      GetAudio().FadeoutMusic(kHurryUpTimeLimit * 1000);
      GetAudio().PlaySound(HurryUp);
    }
  }

  virtual bool IsReady() override { return timer_ == kGameTime; }

  int GetTimeLeft() const { return kGameTime - timer_; }

  bool ShouldPlayHurryUp() {
    if (!hurry_up && GetTimeLeft() <= kHurryUpTimeLimit) {
      hurry_up = true;
      return true;
    }
    return false;
  }

private:
  int frame_ = 0;
  double animation_ticks_ = 0.0;
  double movement_ticks_ = 0.0;
  size_t timer_ = 0;
  size_t step_ = 0;
  bool hurry_up = false;
  std::vector<SDL_Texture *> star_textures_;
  const std::vector<std::pair<int, int>> coordinates_ = {
      std::make_pair(262, 555), std::make_pair(258, 552),
      std::make_pair(256, 548), std::make_pair(253, 545),
      std::make_pair(252, 540), std::make_pair(254, 535),
      std::make_pair(251, 530), std::make_pair(253, 525),
      std::make_pair(251, 522), std::make_pair(254, 520),
      std::make_pair(250, 518), std::make_pair(245, 513),
      std::make_pair(240, 513), std::make_pair(235, 514),
      std::make_pair(230, 511), std::make_pair(228, 509),
      std::make_pair(225, 511), std::make_pair(220, 512),
      std::make_pair(217, 510), std::make_pair(218, 508),
      std::make_pair(216, 507), std::make_pair(215, 506),
      std::make_pair(213, 505), std::make_pair(212, 504),
      std::make_pair(210, 503), std::make_pair(212, 502),
      std::make_pair(214, 500), std::make_pair(210, 495),
      std::make_pair(212, 490), std::make_pair(211, 485),
      std::make_pair(212, 480), std::make_pair(210, 475),
      std::make_pair(212, 470), std::make_pair(210, 465),
      std::make_pair(211, 460), std::make_pair(212, 455),
      std::make_pair(213, 450), std::make_pair(210, 445),
      std::make_pair(211, 440), std::make_pair(212, 435),
      std::make_pair(210, 430), std::make_pair(208, 425),
      std::make_pair(210, 420), std::make_pair(208, 415),
      std::make_pair(210, 410), std::make_pair(208, 405),
      std::make_pair(210, 400), std::make_pair(208, 395),
      std::make_pair(210, 390), std::make_pair(210, 385),
      std::make_pair(209, 380), std::make_pair(207, 375),
      std::make_pair(200, 372), std::make_pair(196, 372),
      std::make_pair(192, 372), std::make_pair(188, 372),
      std::make_pair(185, 372), std::make_pair(183, 372),
      std::make_pair(181, 372), std::make_pair(179, 372),
  };
};

class ExplosionAnimation final : public Animation {
public:
  ExplosionAnimation(SDL_Renderer *renderer, Grid &grid,
                     std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), explosion_texture_(asset_manager->GetExplosionTextures()) {}

  virtual void Start() override {}

  virtual void Update(double delta) override {
    const SDL_Rect rc{100, 278, 71, 100};

    RenderCopy(explosion_texture_.at(frame_), rc);
    animation_ticks_ += delta;
    if (animation_ticks_ >= (kTimeResolution * 5)) {
      frame_++;
      animation_ticks_ = 0.0;
    }
  }

  virtual bool IsReady() override {
    return (static_cast<size_t>(frame_) >= explosion_texture_.size());
  }

private:
  int frame_ = 0;
  double animation_ticks_ = 0.0;
  std::vector<SDL_Texture *> explosion_texture_;
};

class ThresholdReachedAnimation final : public Animation {
public:
  ThresholdReachedAnimation(SDL_Renderer *renderer, Grid &grid,
                            std::shared_ptr<AssetManager> &asset_manager, int value)
      : Animation(renderer, grid, asset_manager) {
    const std::string kText = std::to_string(value - (value % kThresholdMultiplier)) + " diamonds cleared";

    int width, height;
    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Large), kText, Color::White);

    rc_ = { kBlackAreaX + Center(kBlackAreadWidth, width), kBlackAreaY + Center(kBlackAreadHeight, height) , width, height };
  }

  ~ThresholdReachedAnimation() {  SDL_DestroyTexture(texture_); }

  virtual void Start() override { GetAudio().PlaySound(SoundEffect::ThresholdReached); }

  virtual void Update(double delta) override {
    const double kFade = (ticks_ <= 0.6) ? 0.0 : 500.0;

    SDL_SetTextureAlphaMod(texture_, static_cast<Uint8>(alpha_));
    RenderCopy(texture_, rc_);

    alpha_ -= delta * kFade;
    ticks_ += delta;
  }

  virtual bool IsReady() override {
    if (ticks_ >= 2.0 || alpha_ <= 0) {
      return true;
    }
    return false;
  }

  virtual bool LockBoard() const override { return false; }

private:
  double alpha_ = 255.0;
  SDL_Texture* texture_;
  SDL_Rect rc_;
  double ticks_;
};
