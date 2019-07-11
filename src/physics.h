#ifndef __PHYS__
#define __PHYS__

#include <vector>
#include "geometry.h"

enum collider_type { type_rect = 'r', type_circle = 'c' };

typedef struct physics_object {
	enum collider_type collider;
	rect rect_collider;
	circle circle_collider;
	float movement_angle;
	float movement_speed;
	float mass;
	std::vector<physics_object*> collided;
} physics_object;
physics_object create_rect_physics_object(rect r, float mass, float angle, float speed);
physics_object create_circle_physics_object(circle c, float mass, float angle, float speed);

void update_physics(std::vector<physics_object>* objects);

#endif