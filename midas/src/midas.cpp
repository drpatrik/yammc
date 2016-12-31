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
    Timer idle_timer(kIdleTimer);
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
                board.BoardIsNotIdle();
                idle_timer.Reset();
                animations = board.ButtonPressed(Position(pixel_to_row(event.motion.y), pixel_to_col(event.motion.x)));
                break;
            }
        }
      }
      if (idle_timer.IsZero()) {
        animations = board.BoardIsIdle();
        idle_timer.Reset();
      }
      using namespace std::chrono;

      auto start = HighResClock::now();
      board.Render(animations);
      auto duration = duration_cast<microseconds>(HighResClock::now() - start);
      std::this_thread::sleep_for(microseconds(std::max(static_cast<int64_t>(0), static_cast<int64_t>(kFrameDelay - duration.count()))));
    }
    SDL_Quit();
  }
};

int main(int, char * []) {
  MidasMiner midas_miner;

  midas_miner.Play();

  return 0;
}
