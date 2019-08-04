#ifndef __DRAW__
#define __DRAW__

#include <SDL2/SDL.h>
#include "geometry.h"
#include "physics.h"

void draw_rectangle(SDL_Renderer* renderer, rect r);
void draw_circle(SDL_Renderer* renderer, circle c);
void draw_intersecting_segments(SDL_Renderer* renderer, point a1, point a2, point b1, point b2);
void draw_point_in_rect(SDL_Renderer* renderer, rect r, point p);
void draw_distance_from_point_to_line(SDL_Renderer* renderer, point p, point pl1, point pl2);
void draw_normal_from_point(SDL_Renderer* renderer, physics_object obj, point p);

#endif