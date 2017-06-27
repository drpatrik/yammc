#pragma once

#include "function_caller.h"

#include <vector>
#include <memory>

#include <SDL_mixer.h>

enum SoundEffect {
  DiamondLanding,
  Explosion,
  MoveSuccessful,
  MoveUnSuccessful,
  RemovedOneChain,
  RemovedTwoChains,
  RemovedManyChains,
  ThresholdReached,
  TimesUp,
  Hint,
  HighScore,
  HurryUp
};

class Audio final {
 public:
  Audio();

  ~Audio() noexcept;

  void PlayMusic() const {
    Mix_HaltMusic();
    Mix_RewindMusic();
    Mix_FadeInMusic(music_.get(), -1, 500);
  }

  void FadeoutMusic(int ms) const { Mix_FadeOutMusic(ms); }

  void StopMusic() const {  Mix_HaltMusic(); }

  void PlaySound(SoundEffect effect, int time_in_ms = -1) const {
    Mix_PlayChannelTimed(-1, sound_effects_.at(effect).get(), 0, time_in_ms);
  }

  void StopSound() const { Mix_HaltChannel(-1); }

 private:
  using UniqueMusicPtr = std::unique_ptr<Mix_Music, function_caller<void(Mix_Music*), &Mix_FreeMusic>>;
  using UniqueChunkPtr = std::unique_ptr<Mix_Chunk, function_caller<void(Mix_Chunk*), &Mix_FreeChunk>>;

  UniqueMusicPtr music_;
  std::vector<UniqueChunkPtr> sound_effects_;
};
