#include <SDL.h>
#include <cassert>
#include <fstream>
#include <memory>
#include <sstream>

#include <cxxrtl/cxxrtl_vcd.h>
#include <sevsegsim.h>

#include "render.h"
#include "testbench.h"

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "failed SDL init" << std::endl;
    return 1;
  }
  atexit(SDL_Quit);

  cxxrtl_design::p_top top;

  bool do_vcd = argc >= 3 && std::string(argv[1]) == "--vcd";
  std::string vcd_out = do_vcd ? std::string(argv[2]) : "";
  cxxrtl::vcd_writer vcd;
  uint64_t vcd_time = 0;
  if (do_vcd) {
    debug_items di;
    top.debug_info(&di, nullptr, "top ");
    vcd.add(di);
  }

  auto window = SDL_CreateWindow("sevsegsim", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 600, 300, 0);
  auto renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

  top.p_reset.set(true);
  top.p_clock.set(true);
  top.step();
  vcd.sample(vcd_time++);
  top.p_clock.set(false);
  top.step();
  vcd.sample(vcd_time++);
  top.p_reset.set(false);

  auto finished = false;
  std::string window_title;
  while (!finished) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    bool segments[] = {true, false, true, false, true, false, true};
    for (int i = 0; i < 4; ++i) {
      drawSegments(renderer, segments, 50.f + i * 130.f, 50.f);
    }

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        finished = true;
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
          finished = true;
        break;
      }
    }

    top.p_clock.set(true);
    top.step();
    vcd.sample(vcd_time++);
    top.p_clock.set(false);
    top.step();
    vcd.sample(vcd_time++);

    std::ostringstream oss;
    oss << "sevsegsim - cycle " << (vcd_time >> 1);
    window_title = oss.str();
    SDL_SetWindowTitle(window, window_title.c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  if (do_vcd) {
    std::ofstream of(vcd_out);
    of << vcd.buffer;
  }

  return 0;
}
