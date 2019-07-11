#include <math.h>
#include "physics.h"
#include "geometry.h"

physics_object create_rect_physics_object(rect r, float mass, float angle, float speed) {
	physics_object obj;
	obj.collider = type_rect;
	obj.rect_collider = r;
	obj.mass = mass;
	obj.movement_angle = angle;
	obj.movement_speed = speed;
	return obj;
}
physics_object create_circle_physics_object(circle c, float mass, float angle, float speed) {
	physics_object obj;
	obj.collider = type_circle;
	obj.circle_collider = c;
	obj.mass = mass;
	obj.movement_angle = angle;
	obj.movement_speed = speed;\
	return obj;
}

point get_center(physics_object obj) {
	if(obj.collider == type_circle) {
		return obj.circle_collider.center;
	} else if(obj.collider == type_rect) {
		return obj.rect_collider.center;
	} else {
		printf("PROBLEM - Trying to get center of physics_object that has no collider type.\n");
		return create_point_s(NAN, NAN);
	}
}

bool sort_physics_objects(physics_object a, physics_object b) { return a.mass > b.mass; }

void update_physics(std::vector<physics_object>* objects) {
	sort(objects->begin(), objects->end(), sort_physics_objects);
	for(int i=0; i < objects->size(); i++) {
		objects->at(i).collided.clear();
	}
	for(int i=0; i < objects->size(); i++) {

		physics_object* working_object = &(objects->at(i));
		// move
		if(working_object->collider == type_circle) {
			point new_center = move_point_in_direction(
				working_object->circle_collider.center,
				working_object->movement_speed,
				working_object->movement_angle
			);
			working_object->circle_collider.center.x = new_center.x;
			working_object->circle_collider.center.y = new_center.y;

		} else if(working_object->collider == type_rect) {
			point new_center = move_point_in_direction(
				working_object->rect_collider.center,
				working_object->movement_speed,
				working_object->movement_angle
			);
			working_object->rect_collider.center.x = new_center.x;
			working_object->rect_collider.center.y = new_center.y;
		}

		// check for collisions
		for(int j=i+1; j < objects->size(); j++) {
			physics_object* comparison_object = &(objects->at(j));

			// 0 mass = no collisions
			if(comparison_object->mass > 0 && working_object->mass > 0) {
				physics_object* mover;
				physics_object* pusher;
				if(working_object->mass > comparison_object-> mass) {
					pusher = working_object;
					mover = comparison_object;
				} else {
					pusher = comparison_object;
					mover = working_object;
				}
				point pusher_center = get_center(*pusher);
				point mover_center = get_center(*mover);
				float push_angle = atan2(mover_center.y - pusher_center.y, mover_center.x - pusher_center.x); 
				float push_distance = 0;
				if(mover->collider == type_circle) {
					// circle-circle collision
					if(pusher->collider == type_circle) {
						bool collided = collision_circle_circle(pusher->circle_collider, mover->circle_collider);
						if(collided) {
							// push_distance = (pusher->circle_collider.radius + mover->circle_collider.radius) - (distance(pusher_center, mover_center));
							pusher->collided.push_back(mover);
							mover->collided.push_back(pusher);
						}
					}
					// if(push_distance > 0) {
					// 	point new_position = move_point_in_direction(mover_center, push_distance, push_angle);
					// 	mover->circle_collider.center.x = new_position.x;
					// 	mover->circle_collider.center.y = new_position.y;
					// }
				} else if(mover->collider == type_rect) {
					// rect-rect collision
					if(pusher->collider == type_rect) {
						point intersection_corner = collision_rect_rect(pusher->rect_collider, mover->rect_collider);
						if(!isnan(intersection_corner.x)) {
							pusher->collided.push_back(mover);
							mover->collided.push_back(pusher);
						// 	// find which rect has the corner penetrated by the other
						// 	point pusher_corners[4];
						// 	get_array(get_rect_corners(pusher->rect_collider), pusher_corners);
						// 	point mover_corners[4];
						// 	get_array(get_rect_corners(mover->rect_collider), mover_corners);
						// 	physics_object* stabber = NULL;
						// 	physics_object* stabbed = NULL;
						// 	point* stabber_corners = NULL;
						// 	point* stabbed_corners = NULL;
						// 	for(int i=0; i<4; i++) {
						// 		if(approx_equals(pusher_corners[i], intersection_corner)) {
						// 			stabber = pusher;
						// 			stabber_corners = pusher_corners;
						// 			stabbed = mover;
						// 			stabbed_corners = mover_corners;
						// 		} else if(approx_equals(mover_corners[i], intersection_corner)) {
						// 			stabber = mover;
						// 			stabber_corners = mover_corners;
						// 			stabbed = pusher;
						// 			stabbed_corners = pusher_corners;
						// 		}
						// 	}
						// 	// find which rect edge that the corner penetrates
						// 	point endpoint1;
						// 	point endpoint2;
						// 	for(int i=0; i<4; i++) {
						// 		endpoint1 = stabbed_corners[i];
						// 		int e2_index = i + 1;
						// 		e2_index = e2_index == 5 ? 0 : e2_index;
						// 		endpoint2 = stabbed_corners[e2_index];
						// 		point potential_intersection = get_intersection(endpoint1, endpoint2, intersection_corner, stabber->rect_collider.center, false);
						// 		if(!isnan(potential_intersection.x)) {
						// 			break;
						// 		}
						// 	}
						// 	// find angle & distance of push
						// 	float delta_x = endpoint2.x - endpoint1.x;
						// 	float delta_y = endpoint2.y - endpoint1.y;
						// 	float edge_angle = atan2(delta_y, delta_x);
						// 	push_angle = edge_angle - M_PI/2;
						// 	push_distance = distance_from_point_to_line(endpoint1, endpoint2, intersection_corner);
						}
					}
					// if(push_distance > 0) {
					// 	point new_position = move_point_in_direction(mover_center, push_distance, push_angle);
					// 	mover->rect_collider.center.x = new_position.x;
					// 	mover->rect_collider.center.y = new_position.y;
					// }
				}
			}
		}
	}
}