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

  // It's pretty sketch, but for now we "ghost" the display and therefore mimic
  // human vision by simply not rendering unpowered digits, and not clearing the
  // display between renders. This may not be portable. It actually produces
  // some pretty fun artefacting for me when 'rotating' the display with
  // spacebar/UBTN!

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  bool segments[7];
  std::string window_title;
  while (sc->running()) {
    auto ticks_start = SDL_GetTicks64();

    inst.p_ubtn.set(true);

    for (int i = 0; i < 4; ++i) {
      if ((i == 0 && !inst.p_ds__0) || (i == 1 && !inst.p_ds__1) ||
          (i == 2 && !inst.p_ds__2) || (i == 3 && !inst.p_ds__3)) {
        // off.
      } else {
        segments[0] = !inst.p_abcdefgp__0;
        segments[1] = !inst.p_abcdefgp__1;
        segments[2] = !inst.p_abcdefgp__2;
        segments[3] = !inst.p_abcdefgp__3;
        segments[4] = !inst.p_abcdefgp__4;
        segments[5] = !inst.p_abcdefgp__5;
        segments[6] = !inst.p_abcdefgp__6;
        drawSegments(renderer, segments, 50.f + i * 130.f, 50.f);
      }
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

    auto elapsed = SDL_GetTicks64() - ticks_start;
    std::ostringstream oss;
    oss << "sevsegsim - cycle " << sc->cycle_number() << " / "
        << int(1.f / (elapsed / 1000.f)) << " fps";
    window_title = oss.str();
    SDL_SetWindowTitle(window, window_title.c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  sc->wait();

  return 0;
}
