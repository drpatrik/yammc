#include "audio.h"

#include <iostream>
#include <string>

namespace {

std::vector<std::string> kSoundEffects = {
  "diamond-land.wav",
  "explosion.wav",
  "move-successful.wav",
  "move-unsuccessful.wav",
  "removed-one-chain.wav",
  "removed-two-chains.wav",
  "removed-many-chains.wav",
  "threshold_reached.wav",
  "times-up.wav",
  "hint.wav",
  "high-score.wav"
};

const std::string kAssetFolder = "../../assets/sfx/";

}

Audio::Audio() {
  Mix_AllocateChannels(16);

  std::string full_path = kAssetFolder + "music-loop.wav";

  music_ = Mix_LoadMUS(full_path.c_str());

  if (nullptr == music_) {
    std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
    exit(-1);
  }
  Mix_VolumeMusic(MIX_MAX_VOLUME / 3);

  for (auto effect : kSoundEffects) {
    full_path = kAssetFolder + effect;

    Mix_Chunk *chunk = Mix_LoadWAV(full_path.c_str());

    if (nullptr == chunk) {
      std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
      exit(-1);
    }
    sound_effects_.push_back(chunk);
    Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
  }
}

Audio::~Audio() noexcept {
  if (nullptr != music_) {
    Mix_FreeMusic(music_);
  }
  for (auto effect : sound_effects_) {
    Mix_FreeChunk(effect);
  }
}
