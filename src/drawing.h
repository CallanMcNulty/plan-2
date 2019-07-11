#ifndef __DRAW__
#define __DRAW__

#include "geometry.h"
#include <SDL2/SDL.h>

void draw_rectangle(SDL_Renderer* renderer, rect r);
void draw_circle(SDL_Renderer* renderer, circle c);
void draw_intersecting_segments(SDL_Renderer* renderer, point a1, point a2, point b1, point b2);
void draw_point_in_rect(SDL_Renderer* renderer, rect r, point p);

#endif