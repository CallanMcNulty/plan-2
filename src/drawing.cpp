#include <SDL2/SDL.h>
#include "geometry.h"

void draw_rectangle(SDL_Renderer* renderer, rect r) {
	rect_corners corners = get_rect_corners(r);
	SDL_Point points[5] = {
		{corners.upper_left.x, corners.upper_left.y},
		{corners.upper_right.x, corners.upper_right.y},
		{corners.lower_right.x, corners.lower_right.y},
		{corners.lower_left.x, corners.lower_left.y},
		{corners.upper_left.x, corners.upper_left.y}
	};
	SDL_RenderDrawLines(renderer, points, 5);
}

void draw_circle(SDL_Renderer* renderer, circle c) {
	int vertices_count = 30;
	float angle_size = (2*M_PI)/(vertices_count - 1);

	point p = create_point_s(c.center.x, c.center.y + c.radius);
	SDL_Point sdl_points[vertices_count];
	for(int i=0; i<vertices_count; i++) {
		p = rotate_point_about(p, c.center, angle_size);
		SDL_Point sdl_point;
		sdl_point.x = p.x;
		sdl_point.y = p.y;
		sdl_points[i] = sdl_point;
	}
	SDL_RenderDrawLines(renderer, sdl_points, vertices_count);
}

void draw_intersecting_segments(SDL_Renderer* renderer, point a1, point a2, point b1, point b2) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, a1.x, a1.y, a2.x, a2.y);
	SDL_RenderDrawLine(renderer, b1.x, b1.y, b2.x, b2.y);
	point sectoff = get_intersection(a1, a2, b1, b2, true);
	point sect = get_intersection(a1, a2, b1, b2, false);
	if(!isnan(sectoff.x)) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_Rect sdlrect = {sectoff.x, sectoff.y, 5, 5};
		SDL_RenderFillRect(renderer, &sdlrect);
	}
	if(!isnan(sect.x)) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_Rect sdlrect = {sect.x, sect.y, 5, 5};
		SDL_RenderFillRect(renderer, &sdlrect);
	}
}

void draw_point_in_rect(SDL_Renderer* renderer, rect r, point p) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	draw_rectangle(renderer, r);
	if(point_in_rect(p, get_rect_corners(r))) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
	} else {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	}

	SDL_Rect sdlrect = {p.x, p.y, 5, 5};
	SDL_RenderFillRect(renderer, &sdlrect);
}