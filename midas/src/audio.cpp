#include "audio.h"

#include <iostream>
#include <string>
#include <tuple>

namespace {

const std::vector<std::pair<std::string, int>> kSoundEffects = {
  { "diamond-land.wav", MIX_MAX_VOLUME / 4 },
  { "explosion.wav", MIX_MAX_VOLUME },
  { "move-successful.wav", MIX_MAX_VOLUME / 2 },
  { "move-unsuccessful.wav", MIX_MAX_VOLUME / 2 },
  { "removed-one-chain.wav", MIX_MAX_VOLUME },
  { "removed-two-chains.wav", MIX_MAX_VOLUME },
  { "removed-many-chains.wav", MIX_MAX_VOLUME },
  { "threshold_reached.wav", MIX_MAX_VOLUME },
  { "times-up.wav", MIX_MAX_VOLUME / 2 },
  { "hint.wav", MIX_MAX_VOLUME },
  { "high-score.wav", MIX_MAX_VOLUME },
  { "hurryup.wav", MIX_MAX_VOLUME }
};

const int kMixChannels = 16;
#if defined(__linux__)
const std::string kAssetFolder = "assets/sfx/";
#else
const std::string kAssetFolder = "../../assets/sfx/";
#endif
}

Audio::Audio() {
  std::string full_path = kAssetFolder + "music-loop.wav";

  music_ = UniqueMusicPtr{ Mix_LoadMUS(full_path.c_str()) };

  if (nullptr == music_) {
    std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
    exit(-1);
  }
  Mix_AllocateChannels(kMixChannels);
  Mix_VolumeMusic(MIX_MAX_VOLUME / 3);

  for (auto effect_entry : kSoundEffects) {
    auto [effect, volume] = effect_entry;

    full_path = kAssetFolder + effect;

    auto chunk = UniqueChunkPtr{ Mix_LoadWAV(full_path.c_str()) };

    if (nullptr == chunk) {
      std::cout << "Failed to load: " << full_path << ". Error: " << Mix_GetError() << std::endl;
      exit(-1);
    }
    Mix_VolumeChunk(chunk.get(), volume);
    sound_effects_.push_back(std::move(chunk));

  }
}

Audio::~Audio() noexcept {
  Mix_HaltMusic();
  Mix_HaltChannel(-1);
}
