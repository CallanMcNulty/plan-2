#include <SDL2/SDL.h>
#include <fstream>
#include "editing.h"
#include "controls.h"

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
		sizeof(int) * physics_objects_count // number of movement objects for each physics_object
	;
	// bytecount must also include size of all the movement objects themselves, add them now
	for(int i=0; i<physics_objects_count; i++) {
		byte_count += objects[i].movement.size();
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

void update_editor_state(editor_state* editor, std::vector<physics_object>* objects, controls_state controls) {
	// make selections
	if(controls.left_mouse_button_clicked) {
		for(int i=0; i<objects->size(); i++) {
			if( // object was clicked
				(
					objects->at(i).collider == type_circle &&
					collision_circle_circle(objects->at(i).circle_collider, create_circle_s(controls.mouse_position, 0))
				) || (
					objects->at(i).collider == type_rect &&
					point_in_rect(controls.mouse_position, get_rect_corners(objects->at(i).rect_collider))
				)
			) {
				// check if object was already selected
				bool already_selected = false;
				for(int j=0; j<editor->selected_objects.size(); j++) {
					if(editor->selected_objects[j]->id == objects->at(i).id) {
						already_selected = true;
						break;
					}
				}
				// ad it to selection if it was not
				if(!already_selected) {
					editor->selected_objects.push_back(&objects->at(i));
				}
			}
		}

	// deselect
	} else if(key_control_tapped(controls, CTRL_EDITOR_DESELECT)) {
		editor->selected_objects.clear();

	// move objects
	} else if(controls.left_mouse_button_down && (controls.mouse_moved_x != 0 || controls.mouse_moved_y != 0)) {
		// printf("%f, %f\n", controls.mouse_world_moved_x, controls.mouse_world_moved_y);
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
	}
}