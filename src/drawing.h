#ifndef __DRAW__
#define __DRAW__

#include <SDL2/SDL.h>
#include <string>
#include "geometry.h"
#include "physics.h"
#include "editing.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

point reverse_camera_transform_point(rect camera_rect, point screen_point);
rect camera_transform_rect(rect camera_rect, rect target_rect);
circle camera_transform_circle(rect camera_rect, circle target_circle);

typedef struct sprite_animation {
	float total_seconds;
	float elapsed_seconds;
	SDL_Texture* texture;
	int src_width;
	int src_height;
	std::vector<float> frame_timing;
} sprite_animation;
sprite_animation load_animation_from_disk(SDL_Renderer* renderer, std::string name);
rect get_animation_rect(sprite_animation anim, float size, char dimension);
void draw_sprite_anim(SDL_Renderer* renderer, rect camera_rect, sprite_animation* anim, rect target, float elapsed_time_ms);

void draw_rectangle(SDL_Renderer* renderer, rect camera_rect, rect r, bool highlight_top);
void draw_circle(SDL_Renderer* renderer, rect camera_rect, circle c);
void draw_intersecting_segments(SDL_Renderer* renderer, point a1, point a2, point b1, point b2);
// void draw_point_in_rect(SDL_Renderer* renderer, rect r, point p);
// void draw_distance_from_point_to_line(SDL_Renderer* renderer, point p, point pl1, point pl2);
void draw_normal_from_point(SDL_Renderer* renderer, physics_object obj, point p);

void draw_physics_objects_play_mode(SDL_Renderer* renderer, rect camera_rect, std::vector<physics_object> physics);
void draw_physics_objects_edit_mode(SDL_Renderer* renderer, rect camera_rect, std::vector<physics_object> physics, editor_state ed);

#endif