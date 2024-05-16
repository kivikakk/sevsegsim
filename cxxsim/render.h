#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

void drawSegments(SDL_Renderer *renderer, const bool segments[7], float offX,
                  float offY);

#endif
