#ifndef __CONTROL__
#define __CONTROL__

#include <SDL2/SDL.h>
#include <vector>
#include "geometry.h"

enum key_control {
	CTRL_SWITCH_MODE,
	CTRL_RUN_LEFT,
	CTRL_RUN_RIGHT,
	CTRL_JUMP,
	CTRL_SCROLL_MODIFIER,
	CTRL_EDITOR_DESELECT,
};

typedef struct key_control_mapping {
	SDL_Scancode sdl_code[3];
	enum key_control ctrl;
} key_control_mapping;
key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1, SDL_Scancode code2, SDL_Scancode code3);
key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1, SDL_Scancode code2);
key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1);

const key_control_mapping key_mappings[] = {
	create_key_control_mapping(CTRL_SWITCH_MODE, SDL_SCANCODE_ESCAPE),
	create_key_control_mapping(CTRL_RUN_LEFT, SDL_SCANCODE_LEFT),
	create_key_control_mapping(CTRL_RUN_RIGHT, SDL_SCANCODE_RIGHT),
	create_key_control_mapping(CTRL_JUMP, SDL_SCANCODE_UP),
	create_key_control_mapping(CTRL_SCROLL_MODIFIER, SDL_SCANCODE_LGUI),
	create_key_control_mapping(CTRL_SCROLL_MODIFIER, SDL_SCANCODE_RGUI),
	create_key_control_mapping(CTRL_EDITOR_DESELECT, SDL_SCANCODE_D),
};

typedef struct controls_state {
	std::vector<key_control> held_keys;
	std::vector<key_control> tapped_keys;
	point mouse_position;
	float mouse_moved_x;
	float mouse_moved_y;
	point mouse_world_position;
	float mouse_world_moved_x;
	float mouse_world_moved_y;
	bool left_mouse_button_down;
	bool left_mouse_button_clicked;
	bool right_mouse_button_down;
	bool right_mouse_button_clicked;
} controls_state;

bool key_control_held(controls_state state, key_control ctrl);
bool key_control_tapped(controls_state state, key_control ctrl);
void update_controls_state(controls_state* state, Uint8* sdl_key_states, Uint32 mouse_button_states, point mouse_position, rect camera_rect);

#endif