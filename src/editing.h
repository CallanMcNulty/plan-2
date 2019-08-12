#ifndef __EDIT__
#define __EDIT__

#include <string>
#include "physics.h"
#include "controls.h"

typedef struct editor_state {
	std::vector<physics_object*> selected_objects;
} editor_state;

bool save_physics_objects(std::string filename, std::vector<physics_object> objects);
bool load_physics_objects_from_disk(std::string filename, std::vector<physics_object>* objects);
void update_editor_state(editor_state* editor, std::vector<physics_object>* objects, controls_state controls);

#endif