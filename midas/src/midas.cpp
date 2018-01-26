#include "board.h"
#include "timer.h"

#include <thread>
#include <sstream>

namespace {

void InsertAnimation(std::vector<std::shared_ptr<Animation>>& animations, const std::shared_ptr<Animation>& a) {
  if (a) {
    animations.emplace_back(a);
  }
}

}

class MidasMiner {
 public:
  MidasMiner() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    if (TTF_Init() != 0) {
      std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
      exit(-1);
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
      std::cout << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
      exit(-1);
    }
  }

  ~MidasMiner() {
    SDL_Quit();
    Mix_CloseAudio();
    Mix_Quit();
  }

  static void Play() {
    Board board;
    bool quit = false;
    bool music_on = true;
    Timer show_hint_timer(kShowHintTimer);
    Timer idle_penalty_timer(kIdlePenaltyTimer);
    DeltaTimer delta_timer;
    std::vector<std::shared_ptr<Animation>> animations;

    while (!quit) {
      SDL_Event event;

      animations.clear();
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
          break;
        }
        switch (event.type) {
          case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              board.Restart(music_on);
              animations.clear();
              idle_penalty_timer.Reset();
              show_hint_timer.Reset();
              delta_timer.Reset();
            }
            if (!board.IsGameOver() && event.key.keysym.scancode == SDL_SCANCODE_M) {
              music_on = !music_on;
              if (music_on) {
                board.GetAsset().GetAudio().PlayMusic();
              } else {
                board.GetAsset().GetAudio().StopMusic();
              }
            }
            break;
#if !defined(NDEBUG)
          case SDL_MOUSEMOTION: {

            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
            int row = pixel_to_row(mouseY);
            int col = pixel_to_col(mouseX);
            int id = -1;

            if (row != -1 && col != -1) {
              id = board(row, col);
            }
            std::stringstream ss;
            ss << "X: " << mouseX << " Y: " << mouseY << " Row: " <<  row << " Col: " << col << " Id: " << id;

            SDL_SetWindowTitle(board, ss.str().c_str());
          } break;
#endif
          case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT:
                board.BoardNotIdle();
                idle_penalty_timer.Reset();
                show_hint_timer.Reset();
                animations = board.ButtonPressed(Position(pixel_to_row(event.motion.y), pixel_to_col(event.motion.x)));
                break;
            }
        }
      }
      if (idle_penalty_timer.IsZero()) {
        board.DecreseScore();
        idle_penalty_timer.Reset();
      }
      if (show_hint_timer.IsZero()) {
        InsertAnimation(animations, board.ShowHint());
        show_hint_timer.Reset();
      }
      board.Render(animations, delta_timer.GetDelta());
    }
  }
};

int main(int, char * []) {
  MidasMiner midas_miner;

  midas_miner.Play();

  return 0;
}
