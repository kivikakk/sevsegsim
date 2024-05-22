#include <SDL.h>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>

#include <cxxrtl/cxxrtl_vcd.h>
#include <sevsegsim.h>

#include "SimThread.h"
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

  auto sim = SimThread::start(vcd_out);
  auto &inst = Testbench::inst();

  auto window = SDL_CreateWindow("sevsegsim", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 600, 300, 0);
  auto renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  std::string window_title;
  segment_t segacts[4 * 7];
  auto ticks_last = SDL_GetTicks64();
  auto cycles_last = 0;
  auto frame_count = 0;
  while (sim->lock_if_running()) {
    auto cycles_now = sim->cycle_number();
    inst.segacts(segacts);

    inst.p_ubtn.set(false);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        sim->halt();
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          sim->halt();
          break;
        case SDLK_SPACE:
          inst.p_ubtn.set(true);
          break;
        }
        break;
      }
    }

    sim->unlock();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 4; ++i) {
      drawSegments(renderer, &segacts[i * 7], 50.f + i * 130.f, 50.f);
    }

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);

    ++frame_count;

    auto ticks_now = SDL_GetTicks64();
    if (ticks_now - ticks_last > 1000) {
      auto ticks_elapsed = ticks_now - ticks_last;
      auto cycles_elapsed = cycles_now - cycles_last;
      auto fps = int(float(frame_count) / (ticks_elapsed / 1000.f));
      std::ostringstream oss;
      oss << "sevsegsim - ";
      if (cycles_elapsed < 1000)
        oss << cycles_elapsed << "Hz";
      else if (cycles_elapsed < 1000000)
        oss << (cycles_elapsed / 1000) << "kHz";
      else
        oss << (cycles_elapsed / 1000000) << "MHz";
      oss << " / " << fps << " fps";
      window_title = oss.str();
      SDL_SetWindowTitle(window, window_title.c_str());

      ticks_last = ticks_now;
      cycles_last = cycles_now;
      frame_count = 0;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  int rc = sim->wait();
  delete sim;
  return rc;
}
