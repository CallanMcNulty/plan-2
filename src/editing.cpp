#include <SDL2/SDL.h>
#include <fstream>
#include "editing.h"
#include "controls.h"
#include "drawing.h"

void init_editor_state(editor_state* ed) {
	ed->selected_objects.clear();
	ed->click_mode = CLICK_SELECT;
	ed->rotate_origin = create_point_s(NAN, NAN);
	ed->scale_origin = create_point_s(NAN, NAN);
	ed->box_select_origin = create_point_s(NAN, NAN);
}

bool save_physics_objects(std::string filename, std::vector<physics_object> objects) {
	/*
	Structure:

	int(number of physics objects)
	{
		physics_object
		int(number of collided physics objects)
		int(ids of each * number)
		int(number of movement objects)
		movement(* number)
	}(* number)
	*/

	std::ofstream fs ("assets/"+filename, std::ios::binary);
	if(!fs.is_open()) {
		return false;
	}
	int physics_objects_count = objects.size();
	int byte_count =
		sizeof(int) + // number of objects
		physics_objects_count * sizeof(physics_object) + // objects themselves
		2 * sizeof(int) * physics_objects_count // number of movement objects & collisions for each physics_object
	;
	// bytecount must also include size of all the movement objects themselves & the ids for collided objects, add them now
	for(int i=0; i<physics_objects_count; i++) {
		byte_count += objects[i].movement.size() * sizeof(movement);
		byte_count += objects[i].collided.size() * sizeof(int);
	}
	// build array
	char data[byte_count];
	char* cursor = (char*)&data;
	memcpy(cursor, &physics_objects_count, sizeof(int));
	cursor += sizeof(int);
	for(int i=0; i<physics_objects_count; i++) {
		// object itself
		memcpy(cursor, &objects[i], sizeof(physics_object));
		cursor += sizeof(physics_object);
		// collided objects ids
		int collided_count = objects[i].collided.size();
		memcpy(cursor, &collided_count, sizeof(int));
		cursor += sizeof(int);
		for(int j=0; j<collided_count; j++) {
			memcpy(cursor, &objects[i].collided[j], sizeof(int));
			cursor += sizeof(int);
		}
		// movement objects
		int movement_count = objects[i].movement.size();
		memcpy(cursor, &movement_count, sizeof(int));
		cursor += sizeof(int);
		for(int j=0; j<movement_count; j++) {
			memcpy(cursor, &objects[i].movement[j], sizeof(movement));
			cursor += sizeof(movement);
		}
	}
	// write it
	fs.write(data, byte_count);
	fs.close();
	return true;
}

bool load_physics_objects_from_disk(std::string filename, std::vector<physics_object>* objects) {
	std::ifstream fs ("assets/"+filename, std::ios::binary | std::ios::ate);
	if(!fs.is_open()) {
		return false;
	}
	int total_byte_count = fs.tellg();
	char data[total_byte_count];
	fs.seekg(0, std::ios::beg);
	fs.read(data, total_byte_count);
	fs.close();
	char* cursor = (char*)&data;
	int physics_objects_count = *((int*)cursor);
	cursor += sizeof(int);
	for(int i=0; i<physics_objects_count; i++) {
		// object itself
		physics_object obj = *((physics_object*)cursor);
		cursor += sizeof(physics_object);
		// collided object ids
		int collided_count = *((int*)cursor);
		cursor += sizeof(int);
		std::vector<int> new_collided_vec(collided_count);
		for(int j=0; j<collided_count; j++) {
			int collided_id = *((int*)cursor);
			cursor += sizeof(int);
			new_collided_vec.push_back(collided_id);
		}
		obj.collided = new_collided_vec;
		// movement objects
		int movement_count = *((int*)cursor);
		cursor += sizeof(int);
		std::vector<movement> new_movement_vec(movement_count);
		for(int j=0; j<movement_count; j++) {
			movement mvmt = *((movement*)cursor);
			cursor += sizeof(movement);
			new_movement_vec.push_back(mvmt);
		}
		obj.movement = new_movement_vec;
		// finish
		obj.space = objects;
		objects->push_back(obj);
	}
	return true;
}

void set_scale_origin(editor_state* editor, point p) {
	editor->scale_origin = p;
	editor->click_mode = CLICK_SCALE_DRAG;
}

void set_rotate_origin(editor_state* editor, point p) {
	editor->rotate_origin = p;
	editor->click_mode = CLICK_ROTATE_DRAG;
}

void update_selection_box(editor_state* editor, controls_state controls, rect camera_rect) {
	point screen_origin = camera_transform_point(camera_rect, editor->box_select_origin);
	point vert_from_origin = create_point_s(screen_origin.x, controls.mouse_position.y);
	point horiz_from_origin = create_point_s(controls.mouse_position.x, screen_origin.y);
	rect_corners corners;
	if(controls.mouse_position.x < screen_origin.x) {
		if(controls.mouse_position.y < screen_origin.y) {
			corners.upper_left = reverse_camera_transform_point(camera_rect, controls.mouse_position);
			corners.lower_left = reverse_camera_transform_point(camera_rect, horiz_from_origin);
			corners.upper_right = reverse_camera_transform_point(camera_rect, vert_from_origin);
			corners.lower_right = reverse_camera_transform_point(camera_rect, screen_origin);
		} else {
			corners.upper_left = reverse_camera_transform_point(camera_rect, horiz_from_origin);
			corners.lower_left = reverse_camera_transform_point(camera_rect, controls.mouse_position);
			corners.upper_right = reverse_camera_transform_point(camera_rect, screen_origin);
			corners.lower_right = reverse_camera_transform_point(camera_rect, vert_from_origin);
		}
	} else {
		if(controls.mouse_position.y < screen_origin.y) {
			corners.upper_left = reverse_camera_transform_point(camera_rect, vert_from_origin);
			corners.lower_left = reverse_camera_transform_point(camera_rect, screen_origin);
			corners.upper_right = reverse_camera_transform_point(camera_rect, controls.mouse_position);
			corners.lower_right = reverse_camera_transform_point(camera_rect, horiz_from_origin);
		} else {
			corners.upper_left = reverse_camera_transform_point(camera_rect, screen_origin);
			corners.lower_left = reverse_camera_transform_point(camera_rect, vert_from_origin);
			corners.upper_right = reverse_camera_transform_point(camera_rect, horiz_from_origin);
			corners.lower_right = reverse_camera_transform_point(camera_rect, controls.mouse_position);
		}
	}
	editor->selection_box = get_rect_from_corners(corners);
}

bool object_is_selected(editor_state* editor, physics_object obj) {
	bool already_selected = false;
	for(int j=0; j<editor->selected_objects.size(); j++) {
		if(editor->selected_objects[j]->id == obj.id) {
			already_selected = true;
			break;
		}
	}
	return already_selected;
}

void update_editor_state(editor_state* editor, std::vector<physics_object>* objects, controls_state controls, rect camera_rect) {
	// right click
	if(
		controls.right_mouse_button_clicked || (controls.left_mouse_button_clicked && key_control_held(controls, CTRL_RIGHT_MB_MODIFIER))
	) {
		for(int i=0; i<objects->size(); i++) {
			if( // object was clicked
				(
					objects->at(i).collider == type_circle &&
					collision_circle_circle(objects->at(i).circle_collider, create_circle_s(controls.mouse_world_position, 0))
				) || (
					objects->at(i).collider == type_rect &&
					point_in_rect(controls.mouse_world_position, get_rect_corners(objects->at(i).rect_collider))
				)
			) {
				// check if object is selected, and remove it
				for(int j=(editor->selected_objects.size() - 1); j>=0; j--) {
					if(editor->selected_objects[j]->id == objects->at(i).id) {
						editor->selected_objects.erase(editor->selected_objects.begin() + j);
					}
				}
			}
		}

	// left click
	} else if(controls.left_mouse_button_clicked) {
		switch(editor->click_mode) {

			case(CLICK_SELECT): {
				bool object_was_clicked = false;
				for(int i=0; i<objects->size(); i++) {
					if( // object was clicked
						(
							objects->at(i).collider == type_circle &&
							collision_circle_circle(objects->at(i).circle_collider, create_circle_s(controls.mouse_world_position, 0))
						) || (
							objects->at(i).collider == type_rect &&
							point_in_rect(controls.mouse_world_position, get_rect_corners(objects->at(i).rect_collider))
						)
					) {
						object_was_clicked = true;
						// check if object was already selected and add it to selection
						if(!object_is_selected(editor, objects->at(i))) {
							// select only this one if not in multi-select mode
							if(!key_control_held(controls, CTRL_EDITOR_MULTISELECT_MODIFIER)) {
								editor->selected_objects.clear();
							}
							editor->selected_objects.push_back(&objects->at(i));

						// if it was, we're in move mode
						} else {
							editor->click_mode = CLICK_MOVE_OBJECTS;
						}
					}
				}
				if(!object_was_clicked) {
					if(!key_control_held(controls, CTRL_EDITOR_MULTISELECT_MODIFIER)) {
						editor->selected_objects.clear();
					}
				}
				editor->box_select_origin = controls.mouse_world_position;
				update_selection_box(editor, controls, camera_rect);
				break;
			}

			case(CLICK_SET_ROTATE_ORIGIN): {
				set_rotate_origin(editor, controls.mouse_world_position);
				break;
			}

			case(CLICK_SET_SCALE_ORIGIN): {
				set_scale_origin(editor, controls.mouse_world_position);
				break;
			}
		}

	// left click released
	} else if(controls.left_mouse_button_released && !key_control_held(controls, CTRL_RIGHT_MB_MODIFIER)) {
		switch(editor->click_mode) {
			// box select
			case(CLICK_SELECT): {
				if(!key_control_held(controls, CTRL_EDITOR_MULTISELECT_MODIFIER)) {
					editor->selected_objects.clear();
				}
				for(int i=0; i<objects->size(); i++) {
					if(
						!object_is_selected(editor, objects->at(i)) && ((
							objects->at(i).collider == type_circle &&
							collision_circle_rect(objects->at(i).circle_collider, get_rect_corners(editor->selection_box))
						) || (
							objects->at(i).collider == type_rect &&
							!isnan(collision_rect_rect(objects->at(i).rect_collider, editor->selection_box).x)
						))
					) {
						editor->selected_objects.push_back(&objects->at(i));
					}
				}
				break;
			}
			case(CLICK_MOVE_OBJECTS): {
				editor->click_mode = CLICK_SELECT;
			}
			default:
				break;
		}
		editor->box_select_origin.x = NAN;
		editor->box_select_origin.y = NAN;

	// deselect
	} else if(key_control_tapped(controls, CTRL_EDITOR_DESELECT)) {
		editor->selected_objects.clear();

	// click & drag mouse
	} else if(controls.left_mouse_button_down && (controls.mouse_moved_x != 0 || controls.mouse_moved_y != 0)) {
		switch(editor->click_mode) {

			case(CLICK_SELECT): {
				update_selection_box(editor, controls, camera_rect);
				break;
			}

			// move objects
			case(CLICK_MOVE_OBJECTS): {
				for(int i=0; i<editor->selected_objects.size(); i++) {
					physics_object* obj = editor->selected_objects[i];
					if(obj->collider == type_rect) {
						obj->rect_collider.center.x += controls.mouse_world_moved_x;
						obj->rect_collider.center.y += controls.mouse_world_moved_y;
					} else {
						obj->circle_collider.center.x += controls.mouse_world_moved_x;
						obj->circle_collider.center.y += controls.mouse_world_moved_y;
					}
				}
				break;
			}

			case(CLICK_ROTATE_DRAG): {
				point previous_mouse_point = create_point_s(
					controls.mouse_world_position.x - controls.mouse_world_moved_x,
					controls.mouse_world_position.y - controls.mouse_world_moved_y
				);
				float previous_angle = atan2(
					previous_mouse_point.y - editor->rotate_origin.y,
					previous_mouse_point.x - editor->rotate_origin.x
				);
				float current_angle = atan2(
					controls.mouse_world_position.y - editor->rotate_origin.y,
					controls.mouse_world_position.x - editor->rotate_origin.x
				);
				float delta_angle = current_angle - previous_angle;
				for(int i=0; i<editor->selected_objects.size(); i++) {
					physics_object* obj = editor->selected_objects[i];
					if(obj->collider == type_circle) {
						point new_pos = rotate_point_about(obj->circle_collider.center, editor->rotate_origin, delta_angle);
						obj->circle_collider.center.x = new_pos.x;
						obj->circle_collider.center.y = new_pos.y;
					} else {
						rect new_rect = get_rect_from_corners(rotate_corners_about(
							get_rect_corners(obj->rect_collider), editor->rotate_origin, delta_angle
						));
						obj->rect_collider.center.x = new_rect.center.x;
						obj->rect_collider.center.y = new_rect.center.y;
						obj->rect_collider.angle = new_rect.angle;
					}
				}
				break;
			}

			case(CLICK_SCALE_DRAG): {
				float dist = distance(controls.mouse_world_position, editor->scale_origin);
				point prev_mouse_position = create_point_s(
					controls.mouse_world_position.x - controls.mouse_world_moved_x,
					controls.mouse_world_position.y - controls.mouse_world_moved_y
				);
				float prev_dist = distance(prev_mouse_position, editor->scale_origin);
				float factor = dist / prev_dist;
				for(int i=0; i<editor->selected_objects.size(); i++) {
					physics_object* obj = editor->selected_objects[i];
					if(obj->collider == type_circle) {
						obj->circle_collider.center.x -= editor->scale_origin.x;
						obj->circle_collider.center.y -= editor->scale_origin.y;
						obj->circle_collider.center.x *= factor;
						obj->circle_collider.center.y *= factor;
						obj->circle_collider.radius *= factor;
						obj->circle_collider.center.x += editor->scale_origin.x;
						obj->circle_collider.center.y += editor->scale_origin.y;
					} else {
						obj->rect_collider.center.x -= editor->scale_origin.x;
						obj->rect_collider.center.y -= editor->scale_origin.y;
						obj->rect_collider.center.x *= factor;
						obj->rect_collider.center.y *= factor;
						obj->rect_collider.width *= factor;
						obj->rect_collider.height *= factor;
						obj->rect_collider.center.x += editor->scale_origin.x;
						obj->rect_collider.center.y += editor->scale_origin.y;
					}
				}
				break;
			}
			default:
				break;
		}

	// left button held
	} else if(controls.left_mouse_button_down) {
		switch(editor->click_mode) {

			case(CLICK_SELECT): {
				update_selection_box(editor, controls, camera_rect);
				break;
			}
		}

	// save
	} else if(key_control_tapped(controls, CTRL_EDITOR_SAVE)) {
		save_physics_objects("objects.data", *objects);

	// return to default state
	} else if(key_control_tapped(controls, CTRL_EDITOR_FINISH)) {
		init_editor_state(editor);

	// begin scale
	} else if(key_control_tapped(controls, CTRL_EDITOR_SCALE_SELECTION)) {
		editor->click_mode = CLICK_SET_SCALE_ORIGIN;

	// begin rotate
	} else if(key_control_tapped(controls, CTRL_EDITOR_ROTATE_SELECTION)) {
		editor->click_mode = CLICK_SET_ROTATE_ORIGIN;

	// cycle
	} else if(key_control_tapped(controls, CTRL_EDITOR_CYCLE)) {
		bool rotation_mode = editor->click_mode == CLICK_SET_ROTATE_ORIGIN || editor->click_mode == CLICK_ROTATE_DRAG;
		bool scale_mode = editor->click_mode == CLICK_SET_SCALE_ORIGIN || editor->click_mode == CLICK_SCALE_DRAG;
		if(rotation_mode || scale_mode) {
			point origin_point = scale_mode ? editor->scale_origin : editor->rotate_origin;
			int center_on_index = 0;
			for(int i=0; i<editor->selected_objects.size(); i++) {
				point obj_center = get_center(*editor->selected_objects[i]);
				if(approx_equals(obj_center, origin_point)) {
					center_on_index = i + 1;
					break;
				}
			}
			if(center_on_index == editor->selected_objects.size()) {
				editor->click_mode = scale_mode ? CLICK_SET_SCALE_ORIGIN : CLICK_SET_ROTATE_ORIGIN;
			} else if(scale_mode) {
				set_scale_origin(editor, get_center(*editor->selected_objects[center_on_index]));
			} else if(rotation_mode) {
				set_rotate_origin(editor, get_center(*editor->selected_objects[center_on_index]));
			}
		}
	}
}