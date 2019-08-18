#include "controls.h"
#include "drawing.h"

key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1, SDL_Scancode code2, SDL_Scancode code3) {
	key_control_mapping mapping;
	mapping.sdl_code[0] = code1;
	mapping.sdl_code[1] = code2;
	mapping.sdl_code[2] = code3;
	mapping.ctrl = ctrl;
	return mapping;
}
key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1, SDL_Scancode code2) {
	return create_key_control_mapping(ctrl, code1, code2, SDL_SCANCODE_UNKNOWN);
}
key_control_mapping create_key_control_mapping(key_control ctrl, SDL_Scancode code1) {
	return create_key_control_mapping(ctrl, code1, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN);
}

bool key_control_held(controls_state state, key_control ctrl) {
	for(int i=0; i<state.held_keys.size(); i++) {
		if(state.held_keys[i] == ctrl) {
			return true;
		}
	}
	return false;
}

bool key_control_tapped(controls_state state, key_control ctrl) {
	for(int i=0; i<state.tapped_keys.size(); i++) {
		if(state.tapped_keys[i] == ctrl) {
			return true;
		}
	}
	return false;
}

void update_controls_state(controls_state* state, Uint8* sdl_key_states, Uint32 mouse_button_states, point mouse_position, rect camera_rect) {
	// keys
	state->tapped_keys.clear();
	std::vector<key_control> new_held_keys;
	// iterate through key mappings to see if any control is currently pressed
	for(int i=0; i<sizeof(key_mappings)/sizeof(key_control_mapping); i++) {
		bool active = true;
		for(int j=0; j<3; j++) {
			Uint8 sdl_code = key_mappings[i].sdl_code[j];
			if(!sdl_key_states[sdl_code] && sdl_code != SDL_SCANCODE_UNKNOWN) {
				active = false;
				break;
			}
		}
		if(active) {
			// if it is, check if it's already in the held keys list. If not, it was just tapped.
			new_held_keys.push_back(key_mappings[i].ctrl);
			bool pressed_previous_frame = false;
			for(int j=0; j<state->held_keys.size(); j++) {
				if(state->held_keys[j] == key_mappings[i].ctrl) {
					pressed_previous_frame = true;
					break;
				}
			}
			if(!pressed_previous_frame) {
				state->tapped_keys.push_back(key_mappings[i].ctrl);
			}
		}
	}
	state->held_keys = new_held_keys;

	// mouse
	bool left = mouse_button_states & SDL_BUTTON(SDL_BUTTON_LEFT);
	state->left_mouse_button_clicked = left && !state->left_mouse_button_down;
	state->left_mouse_button_released = !left && state->left_mouse_button_down;
	state->left_mouse_button_down = left;

	bool right = mouse_button_states & SDL_BUTTON(SDL_BUTTON_RIGHT);
	state->right_mouse_button_clicked = right && !state->right_mouse_button_down;
	state->right_mouse_button_clicked = !right && state->right_mouse_button_down;
	state->right_mouse_button_down = right;

	state->mouse_moved_x = mouse_position.x - state->mouse_position.x;
	state->mouse_moved_y = mouse_position.y - state->mouse_position.y;
	state->mouse_position = mouse_position;

	point new_mouse_world_position = reverse_camera_transform_point(camera_rect, mouse_position);
	state->mouse_world_moved_x = new_mouse_world_position.x - state->mouse_world_position.x;
	state->mouse_world_moved_y = new_mouse_world_position.y - state->mouse_world_position.y;
	state->mouse_world_position = new_mouse_world_position;
}