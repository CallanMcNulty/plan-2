/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <chrono>
#include "geometry.h"
#include "drawing.h"
#include "physics.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int MS_PER_FRAME = 30;

long get_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;
}

int main(int argc, char* args[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Plan 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return 1;
	}


	std::vector<physics_object> physics(4);
	physics[0] = create_circle_physics_object( create_circle_s(create_point_s(0, 20), 100), 2, 0, 10 );
	physics[1] = create_circle_physics_object( create_circle_s(create_point_s(700, 100), 50), 3, 0, 0 );
	physics[2] = create_rect_physics_object( create_rect_s(create_point_s(300, 350), 100, 50, 1), 2, 0, 2 );
	physics[3] = create_rect_physics_object( create_rect_s(create_point_s(700, 400), 50, 30, 0), 3, 0, 0 );

	point a1 = {50, 200};
	point a2 = {150, 300};
	point b1 = {150, 300};
	point b2 = {100, 100};
	rect r = create_rect_s(create_point_s(50, 500), 50, 50, M_PI/4);
	point p = create_point_s(0, 500);

	long previous_time = get_time();
	long current_time = previous_time;

	SDL_Event e;
	bool quit = false;
	while(!quit) {
		current_time = get_time();
		long elsapsed_time = current_time - previous_time;

		if(elsapsed_time >= MS_PER_FRAME) {
			previous_time = current_time;
			// printf("%ld fps\n", 1000/elsapsed_time);
			while(SDL_PollEvent(&e)) {
				if(e.type == SDL_QUIT) {
					quit = true;
				}
			}
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);

			update_physics(&physics);
			// a1.x ++;
			// a2.x ++;
			// p.x ++;

			//Render stuff
			// for(int i=0; i<SCREEN_WIDTH; i+=10) {
			// 	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			// 	if(i % 50 == 0) {
			// 		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			// 	}
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
					draw_circle(renderer, physics[i].circle_collider);
				} else {
					draw_rectangle(renderer, physics[i].rect_collider);
				}
			}
			// draw_intersecting_segments(renderer, a1, a2, b1, b2);
			// draw_point_in_rect(renderer, r, p);

			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
