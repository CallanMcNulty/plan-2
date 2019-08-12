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
#include "editing.h"
#include "controls.h"

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

	std::vector<physics_object> camera_space(1);
	physics_object camera = create_rect_physics_object(
		&camera_space, create_rect_s(create_point_s(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), SCREEN_WIDTH, SCREEN_HEIGHT, 0), NO_COLLISION
	);
	// camera.movement.push_back(create_movement(1, 5, 1000));

	sprite_animation anim = load_animation_from_disk(renderer, "anim");
	// set_animation_size(&anim, 200, 'b');
	rect anim_rect = get_animation_rect(anim, 200, 'b');
	anim_rect.center.x = 505;
	anim_rect.center.y = 350;

	std::vector<physics_object> physics;
	// physics[0] = create_circle_physics_object(&physics, create_circle_s(create_point_s(500, 350), 100), FIXED_COLLISION );
	// physics[1] = create_circle_physics_object(&physics, create_circle_s(create_point_s(300, 100), 10), DYNAMIC_COLLISION );
	// physics[2] = create_rect_physics_object(&physics, create_rect_s(create_point_s(150, 170), 100, 50, 1), FIXED_COLLISION );
	// physics[3] = create_rect_physics_object(&physics, create_rect_s(create_point_s(700, 400), 50, 70, 0), DYNAMIC_COLLISION );
	// physics[4] = create_rect_physics_object(&physics, create_rect_s(create_point_s(1000, 300), 120, 90, .5), FIXED_COLLISION );
	// physics[0].movement.push_back(create_movement(0, 10));
	// physics[1].movement.push_back(create_movement(0, 0));
	// physics[2].movement.push_back(create_movement(0, 2));
	// physics[2].movement.push_back(create_movement(M_PI/2, .5));
	// physics[3].movement.push_back(create_movement(0, 0));
	// physics[1].grav_attractor = &physics[2];
	// physics[3].grav_attractor = physics[0].id;

	// save_physics_objects("objects.data", physics);
	// physics.clear();
	load_physics_objects_from_disk("objects.data", &physics);

	point a1 = {50, 200};
	point a2 = {150, 300};
	point b1 = {150, 300};
	point b2 = {100, 100};
	rect r = create_rect_s(create_point_s(100, 500), 50, 50, M_PI/4);
	point p = create_point_s(0, 500);

	editor_state editor;
	controls_state controls;

	char mode = 'g';

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

			int mouse_x, mouse_y;
			update_controls_state(
				&controls,
				(Uint8*)SDL_GetKeyboardState(NULL),
				SDL_GetMouseState(&mouse_x, &mouse_y),
				create_point_s((float)mouse_x, (float)mouse_y),
				camera.rect_collider
			);

			while(SDL_PollEvent(&e)) {
				if(e.type == SDL_QUIT) {
					quit = true;
				}
				if(e.type == SDL_MOUSEWHEEL) {
					if(key_control_held(controls, CTRL_SCROLL_MODIFIER)) {
						float factor = 1 + e.wheel.y * -.01;
						camera.rect_collider.width *= factor;
						float new_dist = distance(controls.mouse_world_position, camera.rect_collider.center) * factor;
						float angle = atan2(
							camera.rect_collider.center.y - controls.mouse_world_position.y,
							camera.rect_collider.center.x - controls.mouse_world_position.x
						);
						camera.rect_collider.center = move_point_in_direction(controls.mouse_world_position, new_dist, angle);
					} else {
						int multiplyer = (camera.rect_collider.width/SCREEN_WIDTH) * 10;
						camera.rect_collider.center.x += e.wheel.x * multiplyer;
						camera.rect_collider.center.y += e.wheel.y * multiplyer * -1;
					}
				// } else if(e.type == SDL_KEYDOWN) {
				// 	float screen_portion = .4;
				// 	float cam_height = ((float)SCREEN_HEIGHT/(float)SCREEN_WIDTH)*camera.rect_collider.width;
				// 	if(e.key.keysym.sym == SDLK_KP_8) {
				// 		camera.rect_collider.center.y -= cam_height * screen_portion;
				// 	}
				// 	if(e.key.keysym.sym == SDLK_KP_5) {
				// 		camera.rect_collider.center.y += cam_height * screen_portion;
				// 	}
				// 	if(e.key.keysym.sym == SDLK_KP_4) {
				// 		camera.rect_collider.center.x -= camera.rect_collider.width * screen_portion;
				// 	}
				// 	if(e.key.keysym.sym == SDLK_KP_6) {
				// 		camera.rect_collider.center.x += camera.rect_collider.width * screen_portion;
				// 	}
				}
			}

			if(mode == 'g') {
				// set_movement_along(&physics[1], 5);
				float radial_movement_speed = 0;
				if(key_control_held(controls, CTRL_RUN_LEFT)) {
					radial_movement_speed = -100;
				} else if(key_control_held(controls, CTRL_RUN_RIGHT)) {
					radial_movement_speed = 100;
				}
				if(key_control_tapped(controls, CTRL_JUMP)) {
					jump(&physics[3], 300);
				}
				set_movement_along(&physics[3], radial_movement_speed);

				update_physics(&physics, elapsed_time);

				move(&camera, elapsed_time);
			} else if(mode == 'e') {
				update_editor_state(&editor, &physics, controls);
			}

			if(key_control_tapped(controls, CTRL_SWITCH_MODE)) {
				switch(mode) {
					case('g'):
						mode = 'p';
						break;
					case('p'):
						mode = 'e';
						break;
					case('e'):
						mode = 'g';
						break;
				}
			}

			//Render stuff
			// for(int i=0; i<SCREEN_WIDTH; i+=50) {
			// 	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			// 	if(i % 100 == 0) {
			// 		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
			// 	}
			// 	SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i);
			// 	SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT);
			// }

			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			if(mode == 'e') {
				draw_physics_objects_edit_mode(renderer, camera.rect_collider, physics, editor);
			} else {
				draw_physics_objects_play_mode(renderer, camera.rect_collider, physics);
			}

			draw_sprite_anim(renderer, camera.rect_collider, &anim, anim_rect, elapsed_time);

			// draw a colored border to show mode (none for play, red for paused, orange for editing)
			if(mode != 'g') {
				float ratio = camera.rect_collider.width/(float)SCREEN_WIDTH;
				if(mode == 'p') {
					SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
				} else if(mode == 'e') {
					// draw cursor in edit mode
					SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
					draw_circle(renderer, camera.rect_collider, create_circle_s(controls.mouse_world_position, 8*ratio));

					SDL_SetRenderDrawColor(renderer, 0xFF, 0x80, 0x00, 0xFF);
				}
				for(int i=1; i<4; i++) {
					draw_rectangle(
						renderer,
						camera.rect_collider,
						create_rect_s(
							camera.rect_collider.center,
							camera.rect_collider.width - ratio*i,
							camera.rect_collider.width / ((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT) - ratio*i,
							camera.rect_collider.angle
						),
						false
					);
				}
			}

			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
