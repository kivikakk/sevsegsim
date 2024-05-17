#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

typedef uint8_t segment_t;

void drawSegments(SDL_Renderer *renderer, const uint8_t segments[7], float offX,
                  float offY);

#endif
