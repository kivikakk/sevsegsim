#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

#include "Testbench.h"

void drawSegments(SDL_Renderer *renderer, const segment_t segments[7],
                  float offX, float offY);

#endif
