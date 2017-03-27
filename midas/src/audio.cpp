#include "audio.h"

#include <iostream>
#include <string>
#include <unordered_map>

namespace {

std::vector<std::pair<std::string, int>> kSoundEffects = {
  { "diamond-land.wav", MIX_MAX_VOLUME },
  { "explosion.wav", MIX_MAX_VOLUME },
  { "move-successful.wav", MIX_MAX_VOLUME },
  { "move-unsuccessful.wav", MIX_MAX_VOLUME },
  { "removed-one-chain.wav", MIX_MAX_VOLUME },
  { "removed-two-chains.wav", MIX_MAX_VOLUME },
  { "removed-many-chains.wav", MIX_MAX_VOLUME },
  { "threshold_reached.wav", MIX_MAX_VOLUME },
  { "times-up.wav", MIX_MAX_VOLUME },
  { "hint.wav", MIX_MAX_VOLUME },
  { "high-score.wav", MIX_MAX_VOLUME },
  { "hurryup.wav", MIX_MAX_VOLUME / 2 }
};

const int kMixChannels = 16;
const std::string kAssetFolder = "../../assets/sfx/";

}

Audio::Audio() {
  std::string full_path = kAssetFolder + "music-loop.wav";

  music_ = Mix_LoadMUS(full_path.c_str());

  if (nullptr == music_) {
    std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
    exit(-1);
  }
  Mix_AllocateChannels(kMixChannels);
  Mix_VolumeMusic(MIX_MAX_VOLUME / 3);

  std::string effect;
  int volume;

  for (auto effect_entry : kSoundEffects) {
    std::tie(effect, volume) = effect_entry;

    full_path = kAssetFolder + effect;

    Mix_Chunk *chunk = Mix_LoadWAV(full_path.c_str());

    if (nullptr == chunk) {
      std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
      exit(-1);
    }
    sound_effects_.push_back(chunk);
    Mix_VolumeChunk(chunk, volume);
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
