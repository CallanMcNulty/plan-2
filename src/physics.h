#ifndef __PHYS__
#define __PHYS__

#include <vector>
#include "geometry.h"

const int NULL_MOVEMENT = -1;
const int GRAVITY_MOVEMENT = 0;
const int ORBITAL_MOVEMENT = 1;

const float GRAV_ACCEL = -300;

const int NO_COLLISION = -1;
const int FIXED_COLLISION = 0;
const int DYNAMIC_COLLISION = 1;

typedef struct movement {
	int id;
	float angle;
	float speed;
} movement;

movement create_movement(float angle, float speed, float id);

enum collider_type { type_rect = 'r', type_circle = 'c' };

typedef struct physics_object {
	int id;
	enum collider_type collider;
	rect rect_collider;
	circle circle_collider;
	int collision_index;
	int grav_attractor;
	int supporter;
	std::vector<int> collided;
	std::vector<movement> movement;
	std::vector<physics_object>* space;
} physics_object;

physics_object create_rect_physics_object(std::vector<physics_object>* space, rect r, int collision_index);
physics_object create_circle_physics_object(std::vector<physics_object>* space, circle c, int collision_index);
movement* get_movement_by_id(physics_object* obj, int id);
void set_movement_along(physics_object* mover, float speed);
char get_containing_adjacent_rect_for_point(rect r, point p);
float get_normal_for_rectangle_by_direction(rect r, char direction, point p);
float get_normal_at_external_point(physics_object obj, point p);
movement compose(std::vector<movement> movements);
void jump(physics_object* obj, float force);
movement move(physics_object* obj, float elapsed_time);

void update_physics(std::vector<physics_object>* objects, float elapsed_time);

#endif