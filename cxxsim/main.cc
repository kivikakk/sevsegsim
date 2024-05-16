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

  top.p_io__ubtn.set(true);
  top.p_reset.set(true);
  top.p_clock.set(true);
  top.step();
  vcd.sample(vcd_time++);
  top.p_clock.set(false);
  top.step();
  vcd.sample(vcd_time++);
  top.p_reset.set(false);

  auto &inst = Testbench::inst();

  // const bool off[7] = {false, false, false, false, false, false, false};
  bool segments[7];
  auto finished = false;
  std::string window_title;
  while (!finished) {
    top.p_io__ubtn.set(true);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    // SDL_RenderClear(renderer);

    for (int i = 0; i < 4; ++i) {
      if ((i == 0 && !inst.p_ds__0) || (i == 1 && !inst.p_ds__1) ||
          (i == 2 && !inst.p_ds__2) || (i == 3 && !inst.p_ds__3)) {
        // drawSegments(renderer, off, 50.f + i * 130.f, 50.f);
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
        finished = true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          finished = true;
          break;
        case SDLK_SPACE:
          top.p_io__ubtn.set(false);
          break;
        }
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
