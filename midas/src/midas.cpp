#include "board.h"
#include "timer.h"

#include <thread>
#include <sstream>

class MidasMiner {
 public:
  using HighResClock = std::chrono::high_resolution_clock;

  MidasMiner() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    if (TTF_Init() != 0) {
      std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
      exit(-1);
    }
  }

  ~MidasMiner() = default;

  static void Play() {
    Board board;
    bool quit = false;
    Timer hint_timer(kShowHintTimer);
    Timer idle_timer(kIdleTimer);
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
            if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              board.Restart();
              animations.clear();
              idle_timer.Reset();
              hint_timer.Reset();
              delta_timer.Reset();
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
                idle_timer.Reset();
                hint_timer.Reset();
                animations = board.ButtonPressed(Position(pixel_to_row(event.motion.y), pixel_to_col(event.motion.x)));
                break;
            }
        }
      }
      if (idle_timer.IsZero()) {
        board.DecreseScore();
        idle_timer.Reset();
      }
      if (hint_timer.IsZero()) {
        animations = board.ShowHint();
        hint_timer.Reset();
      }
      board.Render(animations, delta_timer.GetDelta());
    }
    SDL_Quit();
  }
};

int main(int, char * []) {
  MidasMiner midas_miner;

  midas_miner.Play();

  return 0;
}
