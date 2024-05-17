#include <SDL.h>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>

#include <cxxrtl/cxxrtl_vcd.h>
#include <sevsegsim.h>

#include "Sim.h"
#include "Testbench.h"
#include "render.h"

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "failed SDL init" << std::endl;
    return 1;
  }
  atexit(SDL_Quit);

  bool do_vcd = argc >= 3 && std::string(argv[1]) == "--vcd";
  auto vcd_out =
      do_vcd ? std::optional<std::string>(std::string(argv[2])) : std::nullopt;

  auto sc = Sim::start(vcd_out);

  auto window = SDL_CreateWindow("sevsegsim", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 600, 300, 0);
  auto renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  auto &inst = Testbench::inst();

  std::string window_title;
  segment_t segacts[4 * 7];
  auto last_update = SDL_GetTicks64();
  auto frame_count = 0;
  while (sc->running()) {
    inst.p_ubtn.set(true);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    inst.segacts(segacts);
    for (int i = 0; i < 4; ++i) {
      drawSegments(renderer, &segacts[i * 7], 50.f + i * 130.f, 50.f);
    }

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        sc->halt();
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          sc->halt();
          break;
        case SDLK_SPACE:
          inst.p_ubtn.set(false);
          break;
        }
        break;
      }
    }

    ++frame_count;

    auto now = SDL_GetTicks64();
    if (now - last_update > 1000) {
      auto elapsed_ticks = now - last_update;
      auto elapsed_cycles = sc->elapsed_cycles();
      auto fps = int(float(frame_count) / (elapsed_ticks / 1000.f));
      std::ostringstream oss;
      oss << "sevsegsim - ";
      if (elapsed_cycles < 1000)
        oss << elapsed_cycles << "Hz";
      else if (elapsed_cycles < 1000000)
        oss << (elapsed_cycles / 1000) << "kHz";
      else
        oss << (elapsed_cycles / 1000000) << "MHz";
      oss << " / " << fps << " fps";
      window_title = oss.str();
      SDL_SetWindowTitle(window, window_title.c_str());

      last_update = now;
      frame_count = 0;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  sc->wait();

  return 0;
}
