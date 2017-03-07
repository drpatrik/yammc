#pragma once

#include <vector>
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
  HighScore
};

class Audio final {
 public:
  Audio();

  ~Audio() noexcept;

  void PlayMusic() const {
    Mix_HaltChannel(-1);
    Mix_RewindMusic();
    Mix_PlayMusic(music_, -1);
  }

  void StopMusic() const {
    Mix_HaltMusic();
  }

  void PlaySound(SoundEffect effect, int time_in_ms = -1) const { Mix_PlayChannelTimed(-1, sound_effects_.at(effect), 0, time_in_ms); }

  void StopSound() const { Mix_HaltChannel(-1); }

 private:
  Mix_Music *music_ = nullptr;
  std::vector<Mix_Chunk*> sound_effects_;
};
