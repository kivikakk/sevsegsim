
#include "render.h"

static SDL_Colour grey = {64, 64, 64, SDL_ALPHA_OPAQUE},
                  red = {255, 64, 64, SDL_ALPHA_OPAQUE};

static int t1[] = {0, 1, 2}, t2[] = {0, 2, 3}, t3[] = {0, 3, 4},
           t4[] = {0, 4, 5};

static SDL_FPoint vps[6][4] = {
    {{5, 0}, {95, 0}, {75, 20}, {25, 20}},
    {{100, 5}, {100, 95}, {80, 75}, {80, 25}},
    {{100, 105}, {100, 195}, {80, 175}, {80, 125}},
    {{95, 200}, {5, 200}, {25, 180}, {75, 180}},
    {{0, 195}, {0, 105}, {20, 125}, {20, 175}},
    {{0, 95}, {0, 5}, {20, 25}, {20, 75}},
};

void drawSegments(SDL_Renderer *renderer, const bool segments[7], float offX,
                  float offY) {
  for (int i = 0; i < 6; ++i) {
    SDL_Vertex vx[] = {{vps[i][0], segments[i] ? red : grey, {}},
                       {vps[i][1], segments[i] ? red : grey, {}},
                       {vps[i][2], segments[i] ? red : grey, {}},
                       {vps[i][3], segments[i] ? red : grey, {}}};
    for (int j = 0; j < 4; ++j)
      vx[j].position.x += offX, vx[j].position.y += offY;

    SDL_RenderGeometry(renderer, nullptr, vx, 4, t1, 3);
    SDL_RenderGeometry(renderer, nullptr, vx, 4, t2, 3);
  }

  SDL_Vertex vx[6] = {
      {{15, 90}, segments[6] ? red : grey, {}},
      {{85, 90}, segments[6] ? red : grey, {}},
      {{95, 100}, segments[6] ? red : grey, {}},
      {{85, 110}, segments[6] ? red : grey, {}},
      {{15, 110}, segments[6] ? red : grey, {}},
      {{5, 100}, segments[6] ? red : grey, {}},
  };
  for (int j = 0; j < 6; ++j)
    vx[j].position.x += offX, vx[j].position.y += offY;
  SDL_RenderGeometry(renderer, nullptr, vx, 6, t1, 3);
  SDL_RenderGeometry(renderer, nullptr, vx, 6, t2, 3);
  SDL_RenderGeometry(renderer, nullptr, vx, 6, t3, 3);
  SDL_RenderGeometry(renderer, nullptr, vx, 6, t4, 3);
}