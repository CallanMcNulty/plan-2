/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <chrono>
#include <string>
#include "geometry.h"
#include "drawing.h"
#include "physics.h"

const int MS_PER_FRAME = 30;

long get_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char* args[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	// If we've got multiple monitors, put it on the secondary one
	int displayIndex = SDL_GetNumVideoDisplays() - 1;
	window = SDL_CreateWindow(
		"Plan 2",
		SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex),
		SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex),
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if(window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return 1;
	}

	physics_object camera = create_rect_physics_object(
		create_rect_s(create_point_s(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), SCREEN_WIDTH, SCREEN_HEIGHT, 0), NO_COLLISION
	);
	// camera.movement.push_back(create_movement(1, 5, 1000));

	sprite_animation anim = load_animation_from_disk(renderer, "anim");
	// set_animation_size(&anim, 200, 'b');
	rect anim_rect = get_animation_rect(anim, 200, 'b');
	anim_rect.center.x = 505;
	anim_rect.center.y = 350;

	std::vector<physics_object> physics(5);
	physics[0] = create_circle_physics_object( create_circle_s(create_point_s(500, 350), 100), FIXED_COLLISION );
	physics[1] = create_circle_physics_object( create_circle_s(create_point_s(300, 100), 10), DYNAMIC_COLLISION );
	physics[2] = create_rect_physics_object( create_rect_s(create_point_s(150, 170), 100, 50, 1), FIXED_COLLISION );
	physics[3] = create_rect_physics_object( create_rect_s(create_point_s(700, 400), 50, 70, 0), DYNAMIC_COLLISION );
	physics[4] = create_rect_physics_object( create_rect_s(create_point_s(1000, 300), 120, 90, .5), FIXED_COLLISION );
	// physics[0].movement.push_back(create_movement(0, 10));
	// physics[1].movement.push_back(create_movement(0, 0));
	// physics[2].movement.push_back(create_movement(0, 2));
	// physics[2].movement.push_back(create_movement(M_PI/2, .5));
	// physics[3].movement.push_back(create_movement(0, 0));
	// physics[1].grav_attractor = &physics[2];
	physics[3].grav_attractor = &physics[0];

	point a1 = {50, 200};
	point a2 = {150, 300};
	point b1 = {150, 300};
	point b2 = {100, 100};
	rect r = create_rect_s(create_point_s(100, 500), 50, 50, M_PI/4);
	point p = create_point_s(0, 500);

	const Uint8* key_states;

	long previous_time = get_time();
	long current_time = previous_time;

	SDL_Event e;
	bool quit = false;
	while(!quit) {
		current_time = get_time();
		long elapsed_time = current_time - previous_time;

		if(elapsed_time >= MS_PER_FRAME) {
			previous_time = current_time;
			// printf("%ld fps\n", 1000/elapsed_time);
			// physics[3].movement.clear();
			while(SDL_PollEvent(&e)) {
				if(e.type == SDL_QUIT) {
					quit = true;
				}
				// else if(e.type == SDL_KEYDOWN) {
				// 	if(e.key.keysym.sym == SDLK_UP) {
				// 		physics[3].movement.push_back(create_movement(M_PI*1.5, 10, -1));
				// 	}
				// 	if(e.key.keysym.sym == SDLK_DOWN) {
				// 		physics[3].movement.push_back(create_movement(M_PI*.5, 10, -1));
				// 	}
				// 	if(e.key.keysym.sym == SDLK_LEFT) {
				// 		physics[3].movement.push_back(create_movement(M_PI, 10, -1));
				// 	}
				// 	if(e.key.keysym.sym == SDLK_RIGHT) {
				// 		physics[3].movement.push_back(create_movement(0, 10, -1));
				// 	}
				// }
				key_states = SDL_GetKeyboardState(NULL);
			}
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			// set_movement_along(&physics[1], 5);
			float radial_movement_speed = 0;
			if(key_states[SDL_SCANCODE_LEFT]) {
				radial_movement_speed = -5;
			} else if(key_states[SDL_SCANCODE_RIGHT]) {
				radial_movement_speed = 5;
			}
			if(key_states[SDL_SCANCODE_UP]) {
				jump(&physics[3], 10);
			}
			set_movement_along(&physics[3], radial_movement_speed);
			update_physics(&physics, elapsed_time);
			// a1.x ++;
			// a2.x ++;
			// p.x ++;

			move(&camera, elapsed_time);
			// camera.rect_collider.angle += .001;
			// camera.rect_collider.width -= 1;

			//Render stuff
			// for(int i=0; i<SCREEN_WIDTH; i+=50) {
			// 	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			// 	if(i % 100 == 0) {
			// 		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
			// 	}
			// 	SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i);
			// 	SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT);
			// }

			for(int i=0; i<physics.size(); i++) {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
				if(physics[i].collided.size() > 0) {
					SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
				}
				if(physics[i].collider == type_circle) {
					draw_circle(renderer, camera.rect_collider, physics[i].circle_collider);
				} else {
					draw_rectangle(renderer, camera.rect_collider, physics[i].rect_collider);
				}
			}

			draw_rectangle(renderer, camera.rect_collider, r);
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '8', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '2', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '4', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '6', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '7', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '9', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '3', 20));
			draw_rectangle(renderer, camera.rect_collider, get_adjacent_rect(r, '1', 20));
			// draw_normal_from_point(renderer, physics[2], physics[3].rect_collider.center);
			// draw_intersecting_segments(renderer, a1, a2, b1, b2);
			// draw_point_in_rect(renderer, r, p);
			// draw_distance_from_point_to_line(renderer, p, a1, a2);
			if(rect_contains_circle(get_rect_corners(physics[2].rect_collider), physics[1].circle_collider)) {
				SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
				SDL_Rect sdlrect = {0, 0, 10, 10};
				SDL_RenderFillRect(renderer, &sdlrect);
			}
			if(circle_contains_circle(physics[0].circle_collider, physics[1].circle_collider)) {
				SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF);
				SDL_Rect sdlrect = {10, 0, 10, 10};
				SDL_RenderFillRect(renderer, &sdlrect);
			}
			if(circle_contains_rect(physics[0].circle_collider, get_rect_corners(physics[3].rect_collider))) {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
				SDL_Rect sdlrect = {20, 0, 10, 10};
				SDL_RenderFillRect(renderer, &sdlrect);
			}

			draw_sprite_anim(renderer, camera.rect_collider, &anim, anim_rect, elapsed_time);

			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
