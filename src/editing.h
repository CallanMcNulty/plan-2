#ifndef __EDIT__
#define __EDIT__

#include <string>
#include "physics.h"
#include "controls.h"

enum click_mode {
	CLICK_SELECT,
	CLICK_SET_ROTATE_ORIGIN,
	CLICK_ROTATE_DRAG,
	CLICK_SET_SCALE_ORIGIN,
	CLICK_SCALE_DRAG,
	CLICK_MOVE_OBJECTS
};

typedef struct editor_state {
	std::vector<physics_object*> selected_objects;
	click_mode click_mode;
	point rotate_origin;
	point scale_origin;
	point box_select_origin;
	rect selection_box;
} editor_state;
void init_editor_state(editor_state* ed);
bool save_physics_objects(std::string filename, std::vector<physics_object> objects);
bool load_physics_objects_from_disk(std::string filename, std::vector<physics_object>* objects);
void update_editor_state(editor_state* editor, std::vector<physics_object>* objects, controls_state controls, rect camera_rect);

#endif