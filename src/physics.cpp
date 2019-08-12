#include <stdlib.h>
#include <math.h>
#include <string>
#include "physics.h"
#include "geometry.h"

int get_id_from_space(std::vector<physics_object> space) {
	int id = 0;
	for(int i=0; i<space.size(); i++) {
		id = std::max(id, space[i].id);
	}
	id ++;
	return id;
}

physics_object create_rect_physics_object(std::vector<physics_object>* space, rect r, int collision_index) {
	physics_object obj;
	obj.id = get_id_from_space(*space);
	obj.collider = type_rect;
	obj.rect_collider = r;
	obj.grav_attractor = -1;
	obj.supporter = -1;
	obj.collision_index = collision_index;
	obj.space = space;
	return obj;
}
physics_object create_circle_physics_object(std::vector<physics_object>* space, circle c, int collision_index) {
	physics_object obj;
	obj.id = get_id_from_space(*space);
	obj.collider = type_circle;
	obj.circle_collider = c;
	obj.grav_attractor = -1;
	obj.supporter = -1;
	obj.collision_index = collision_index;
	obj.space = space;
	return obj;
}

physics_object* get_physics_object_by_id(std::vector<physics_object>* space, int id) {
	for(int i=0; i<space->size(); i++) {
		if(space->at(i).id == id) {
			return &(space->at(i));
		}
	}
	return nullptr;
}

physics_object* get_grav_attractor(physics_object obj) {
	return get_physics_object_by_id(obj.space, obj.grav_attractor);
}
physics_object* get_supporter(physics_object obj) {
	return get_physics_object_by_id(obj.space, obj.supporter);
}
physics_object* get_collided_at_index(physics_object obj, int index) {
	return get_physics_object_by_id(obj.space, obj.collided[index]);
}

movement create_movement(float angle, float speed, float id) {
	movement mvmt;
	mvmt.id = id;
	mvmt.angle = angle;
	mvmt.speed = speed;
	return mvmt;
}

movement* get_movement_by_id(physics_object* obj, int id) {
	for(int i=0; i<obj->movement.size(); i++) {
		movement* m = &(obj->movement.at(i));
		if(m->id == id) {
			return m;
		}
	}
	return nullptr;
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

char get_containing_adjacent_rect_for_point(rect r, point p) {
	float extension_length = distance(r.center, p);
	rect_corners corners = get_rect_corners(r);
	if(point_in_rect(p, corners)) {
		return '5';
	}
	std::string directions = "78963214";
	for(int i=0; i<8; i++) {
		if(point_in_rect(p, get_rect_corners(get_adjacent_rect(r, directions[i], extension_length)))) {
			return directions[i];
		}
	}
	return '0';
}

float get_normal_for_rectangle_by_direction(rect r, char direction, point p) {
	rect_corners corners = get_rect_corners(r);
	switch(direction) {
		case('8') :
			return r.angle - M_PI/2;
		case('2') :
			return r.angle + M_PI/2;
		case('4') :
			return r.angle + M_PI;
		case('6') :
			return r.angle;
		case('7') :
			return atan2(p.y - corners.upper_left.y, p.x - corners.upper_left.x);
		case('9') :
			return atan2(p.y - corners.upper_right.y, p.x - corners.upper_right.x);
		case('3') :
			return atan2(p.y - corners.lower_right.y, p.x - corners.lower_right.x);
		case('1') :
			return atan2(p.y - corners.lower_left.y, p.x - corners.lower_left.x);
	}
	return 100;
}

float get_normal_at_external_point(physics_object obj, point p) {
	if(obj.collider == type_circle) {
		return atan2(p.y - obj.circle_collider.center.y, p.x - obj.circle_collider.center.x);
	} else {
		return get_normal_for_rectangle_by_direction(obj.rect_collider, get_containing_adjacent_rect_for_point(obj.rect_collider, p), p);
	}
}

void set_movement_along(physics_object* mover, float speed) {
	physics_object* object_moved_along = get_supporter(*mover);
	if(object_moved_along == nullptr) {
		object_moved_along = get_grav_attractor(*mover);
	}
	float angle;
	point mover_center = get_center(*mover);
	if(object_moved_along->collider == type_circle) {
		// for moving along a circles, we have to move in a curve
		float delta_angle = speed/distance(mover_center, object_moved_along->circle_collider.center);
		point new_pos = rotate_point_about(mover_center, object_moved_along->circle_collider.center, delta_angle);
		angle = atan2(new_pos.y - mover_center.y, new_pos.x - mover_center.x);
		speed = distance(mover_center, new_pos);
	} else {
		// for rectangle, move perpendicular to the normal for movement along an edge then in a curve for movement around corners
		rect_corners corners = get_rect_corners(object_moved_along->rect_collider);
		char collision_direction = get_containing_adjacent_rect_for_point(object_moved_along->rect_collider, mover_center);
		std::string cardinal_directions = "8462";
		if(cardinal_directions.find(collision_direction) != std::string::npos) {
			angle = get_normal_for_rectangle_by_direction(object_moved_along->rect_collider, collision_direction, mover_center) + M_PI*.5;
		} else {
			point rotation_corner;
			switch(collision_direction) {
				case('7') : {
					rotation_corner = corners.upper_left;
					break;
				} case('9') : {
					rotation_corner = corners.upper_right;
					break;
				} case('3') : {
					rotation_corner = corners.lower_right;
					break;
				} case('1') : {
					rotation_corner = corners.lower_left;
					break;
				}
			}
			float delta_angle = speed/distance(mover_center, rotation_corner);
			point new_pos = rotate_point_about(mover_center, rotation_corner, delta_angle);
			angle = atan2(new_pos.y - mover_center.y, new_pos.x - mover_center.x);
			speed = distance(mover_center, new_pos);
		}
	}
	movement* mvmt = get_movement_by_id(mover, ORBITAL_MOVEMENT);
	if(mvmt == nullptr) {
		mover->movement.push_back(create_movement(angle, speed, ORBITAL_MOVEMENT));
	} else {
		mvmt->angle = angle;
		mvmt->speed = speed;
	}
}

movement compose(std::vector<movement> movements) {
	if(movements.size() == 1) {
		return movements[0];
	}
	float delta_x = 0;
	float delta_y = 0;
	for(int i=0; i<movements.size(); i++) {
		point working_deltas = decompose(movements[i].speed, movements[i].angle);
		delta_x += working_deltas.x;
		delta_y += working_deltas.y;
	}
	movement result;
	result.id = 0;
	result.speed = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
	result.angle = atan2(delta_y, delta_x);
	return result;
}

void jump(physics_object* obj, float force) {
	if(obj->supporter == -1 || obj->grav_attractor == -1) {
		return;
	}
	movement* vertical = get_movement_by_id(obj, GRAVITY_MOVEMENT);
	if(vertical == nullptr) {
		return;
	}
	vertical->speed += force;
}

movement move(physics_object* obj, float elapsed_time) {
	if(obj->movement.size() > 0) {
		movement mvmt = compose(obj->movement);
		float distance = mvmt.speed * (elapsed_time/1000);
		if(obj->collider == type_circle) {
			point new_center = move_point_in_direction(obj->circle_collider.center, distance, mvmt.angle);
			obj->circle_collider.center.x = new_center.x;
			obj->circle_collider.center.y = new_center.y;

		} else if(obj->collider == type_rect) {
			point new_center = move_point_in_direction(obj->rect_collider.center, distance, mvmt.angle);
			obj->rect_collider.center.x = new_center.x;
			obj->rect_collider.center.y = new_center.y;
		}
		return mvmt;
	}
	return create_movement(NAN, NAN, NULL_MOVEMENT);
}

void update_physics(std::vector<physics_object>* objects, float elapsed_time) {

	// do movement
	movement saved_movement[objects->size()];
	for(int i=0; i < objects->size(); i++) {
		physics_object* working_object = &(objects->at(i));
		point working_center = get_center(*working_object);
		physics_object* working_grav_attractor = get_grav_attractor(*working_object);

		// Fix rotation of rectangle based on attractor
		if(working_object->grav_attractor != -1) {
			working_object->rect_collider.angle = get_normal_at_external_point(*working_grav_attractor, working_center) + M_PI/2;
		}

		// apply gravity
		movement* gravity = get_movement_by_id(working_object, GRAVITY_MOVEMENT);
		if(working_object->supporter != -1) {
			if(gravity != nullptr && gravity->speed < 0) {
				gravity->speed = 0;
			}
		} else if(working_object->grav_attractor != -1) {
			point grav_attractor_center = get_center(*working_grav_attractor);
			float dist_from_grav_x = working_center.x - grav_attractor_center.x;
			float dist_from_grav_y = working_center.y - grav_attractor_center.y;
			float angle_to_grav_attractor = atan2(dist_from_grav_y, dist_from_grav_x);
			if(gravity == nullptr) {
				working_object->movement.push_back(create_movement(angle_to_grav_attractor, GRAV_ACCEL, GRAVITY_MOVEMENT));
			} else {
				gravity->angle = angle_to_grav_attractor;
				gravity->speed += GRAV_ACCEL * (elapsed_time/1000);
			}
		}

		// move
		saved_movement[i] = move(working_object, elapsed_time);
	}
	// Find collisions
	for(int i=0; i<objects->size(); i++) {
		objects->at(i).collided.clear();
	}
	for(int i=0; i < objects->size(); i++) {
		physics_object* working_object = &(objects->at(i));
		point working_center = get_center(*working_object);

		for(int j=i+1; j < objects->size(); j++) {
			physics_object* comparison_object = &(objects->at(j));
			point comp_center = get_center(*comparison_object);

			if(working_object->collider == type_circle && comparison_object->collider == type_circle) {
				// circle-circle collision
				bool collided = collision_circle_circle(comparison_object->circle_collider, working_object->circle_collider);
				if(collided) {
					comparison_object->collided.push_back(working_object->id);
					working_object->collided.push_back(comparison_object->id);
				}
			} else if(working_object->collider == type_rect && comparison_object->collider == type_rect) {
				// rect-rect collision
				point intersection_corner = collision_rect_rect(comparison_object->rect_collider, working_object->rect_collider);
				if(!isnan(intersection_corner.x)) {
					comparison_object->collided.push_back(working_object->id);
					working_object->collided.push_back(comparison_object->id);
				}
			} else {
				// rect-circle collision
				physics_object* rect_obj;
				physics_object* circle_obj;
				if(working_object->collider == type_circle) {
					circle_obj = working_object;
					rect_obj = comparison_object;
				} else {
					circle_obj = comparison_object;
					rect_obj = working_object;
				}
				if(collision_circle_rect(circle_obj->circle_collider, get_rect_corners(rect_obj->rect_collider))) {
					comparison_object->collided.push_back(working_object->id);
					working_object->collided.push_back(comparison_object->id);
				}
			}
		}
	}
	// Process collisions
	for(int i=0; i < objects->size(); i++) {
		physics_object* working_object = &(objects->at(i));
		physics_object* working_grav_attractor = get_grav_attractor(*working_object);

		bool still_supported = false;

		bool has_solid_collissions = true;
		bool collisions_processed = working_object->collided.size() == 0;
		while(!collisions_processed && has_solid_collissions) {
			has_solid_collissions = false;
			for(int j=0; j < working_object->collided.size(); j++) {
				point working_center = get_center(*working_object);

				physics_object* comparison_object = get_collided_at_index(*working_object, j);
				point comp_center = get_center(*comparison_object);

				// For solid collisions perform pushback
				bool solid_collision =
					working_object->collision_index == DYNAMIC_COLLISION && comparison_object->collision_index == FIXED_COLLISION;
				if(solid_collision) {
					has_solid_collissions = true;
					point new_position = working_center;

					if(working_object->collider == type_circle && comparison_object->collider == type_circle) {
						// circle - circle
						float push_angle = atan2(working_center.y - comp_center.y, working_center.x - comp_center.x);
						new_position = move_point_in_direction(
							comp_center,
							working_object->circle_collider.radius + comparison_object->circle_collider.radius,
							push_angle
						);

					} else if(working_object->collider == type_rect && comparison_object->collider == type_rect) {
						// rect - rect
						rect_corners working_corners = get_rect_corners(working_object->rect_collider);
						rect_corners comparison_corners = get_rect_corners(comparison_object->rect_collider);
						physics_object* stabber = nullptr;
						physics_object* stabbed = nullptr;
						point stabbing_point;
						point adjacent_point_a;
						point adjacent_point_b;
						if(point_in_rect(working_corners.lower_left, comparison_corners)) {
							stabber = working_object;
							stabbed = comparison_object;
							stabbing_point = working_corners.lower_left;
							adjacent_point_a = working_corners.lower_right;
							adjacent_point_b = working_corners.upper_left;
						} else if(point_in_rect(working_corners.lower_right, comparison_corners)) {
							stabber = working_object;
							stabbed = comparison_object;
							stabbing_point = working_corners.lower_right;
							adjacent_point_a = working_corners.lower_left;
							adjacent_point_b = working_corners.upper_right;
						} else if(point_in_rect(working_corners.upper_right, comparison_corners)) {
							stabber = working_object;
							stabbed = comparison_object;
							stabbing_point = working_corners.upper_right;
							adjacent_point_a = working_corners.lower_right;
							adjacent_point_b = working_corners.upper_left;
						} else if(point_in_rect(working_corners.upper_left, comparison_corners)) {
							stabber = working_object;
							stabbed = comparison_object;
							stabbing_point = working_corners.upper_left;
							adjacent_point_a = working_corners.lower_left;
							adjacent_point_b = working_corners.upper_right;
						} else if(point_in_rect(comparison_corners.lower_left, working_corners)) {
							stabber = comparison_object;
							stabbed = working_object;
							stabbing_point = comparison_corners.lower_left;
							adjacent_point_a = comparison_corners.lower_right;
							adjacent_point_b = comparison_corners.upper_left;
						} else if(point_in_rect(comparison_corners.lower_right, working_corners)) {
							stabber = comparison_object;
							stabbed = working_object;
							stabbing_point = comparison_corners.lower_right;
							adjacent_point_a = comparison_corners.lower_left;
							adjacent_point_b = comparison_corners.upper_right;
						} else if(point_in_rect(comparison_corners.upper_right, working_corners)) {
							stabber = comparison_object;
							stabbed = working_object;
							stabbing_point = comparison_corners.upper_right;
							adjacent_point_a = comparison_corners.lower_right;
							adjacent_point_b = comparison_corners.upper_left;
						} else if(point_in_rect(comparison_corners.upper_left, working_corners)) {
							stabber = comparison_object;
							stabbed = working_object;
							stabbing_point = comparison_corners.upper_left;
							adjacent_point_a = comparison_corners.lower_left;
							adjacent_point_b = comparison_corners.upper_right;
						}
						if(stabber == nullptr) {
							printf("Detected a collision, but couldn't find a contained point");
						}
						std::string directions = "8624";
						float normal_direction;
						point edge_p1;
						point edge_p2;
						rect_corners stabbed_corners = get_rect_corners(stabbed->rect_collider);
						for(int k=0; k<4; k++) {
							normal_direction = get_normal_for_rectangle_by_direction(
								stabbed->rect_collider, directions[k], create_point_s(NAN, NAN)
							);
							float adjacent_angle_a = atan2(stabbing_point.y - adjacent_point_a.y, stabbing_point.x - adjacent_point_a.x);
							float adjacent_angle_b = atan2(stabbing_point.y - adjacent_point_b.y, stabbing_point.x - adjacent_point_b.x);
							float least_angle_between = std::min(
								get_real_angle_between(normal_direction, adjacent_angle_a),
								get_real_angle_between(normal_direction, adjacent_angle_b)
							);
							if(least_angle_between >= M_PI/2) {
								switch(directions[k]) {
									case('8') : {
										edge_p1 = stabbed_corners.upper_left;
										edge_p2 = stabbed_corners.upper_right;
										break;
									} case('6') : {
										edge_p1 = stabbed_corners.lower_right;
										edge_p2 = stabbed_corners.upper_right;
										break;
									} case('4') : {
										edge_p1 = stabbed_corners.lower_left;
										edge_p2 = stabbed_corners.upper_left;
										break;
									} case('2') : {
										edge_p1 = stabbed_corners.lower_left;
										edge_p2 = stabbed_corners.lower_right;
										break;
									}
								}
								break;
							}
						}
						normal_direction += M_PI;
						point edge_intersection = get_intersection(
							edge_p1, edge_p2,
							stabbing_point,
							move_point_in_direction(
								stabbing_point, std::max(stabber->rect_collider.width, stabber->rect_collider.height), normal_direction
							), true
						);
						// - 0.1 to move slightly less than the full distance in order to stay collided next frame
						float movement_distance = distance(stabbing_point, edge_intersection) - 0.1;
						new_position = move_point_in_direction(
							working_center, movement_distance, working_object == stabber ? normal_direction - M_PI : normal_direction
						);

					} else if(working_object->collider != comparison_object->collider) {
						// circle - rect
						bool circle_is_pusher = comparison_object->collider == type_circle;
						rect the_rect = circle_is_pusher ? working_object->rect_collider : comparison_object->rect_collider;
						circle the_circle = circle_is_pusher ? comparison_object->circle_collider : working_object->circle_collider;
						rect_corners r_corners = get_rect_corners(the_rect);
						char collision_direction = get_containing_adjacent_rect_for_point(the_rect, the_circle.center);
						float normal = get_normal_for_rectangle_by_direction(the_rect, collision_direction, the_circle.center);
						bool corner_collided = false;
						point p1;
						point p2;
						bool vertical_collided = false;
						switch(collision_direction) {
							case('8') : {
								p1 = r_corners.upper_left;
								p2 = r_corners.upper_right;
								vertical_collided = true;
								break;
							} case('6') : {
								p1 = r_corners.upper_right;
								p2 = r_corners.lower_right;
								break;
							} case('2') : {
								p1 = r_corners.lower_right;
								p2 = r_corners.lower_left;
								vertical_collided = true;
								break;
							} case('4') : {
								p1 = r_corners.upper_left;
								p2 = r_corners.lower_left;
								break;
							} case('7') : {
								corner_collided = true;
								p1 = r_corners.upper_left;
								break;
							} case('9') : {
								corner_collided = true;
								p1 = r_corners.upper_right;
								break;
							} case('3') : {
								corner_collided = true;
								p1 = r_corners.lower_right;
								break;
							} case('1') : {
								corner_collided = true;
								p1 = r_corners.lower_left;
								break;
							}
						}
						if(corner_collided) {
							if(!circle_is_pusher) {
								new_position = move_point_in_direction(p1, working_object->circle_collider.radius, normal);
							} else {
								new_position = move_point_in_direction(
									working_center,
									comparison_object->circle_collider.radius - distance(the_circle.center, p1),
									normal + M_PI
								);
							}
						} else {
							point point_on_edge = get_intersection(
								p1, p2, the_circle.center,
								move_point_in_direction(the_circle.center, the_circle.radius, normal + M_PI),
								false
							);
							if(!isnan(point_on_edge.x)) {
								if(!circle_is_pusher) {
									new_position = move_point_in_direction(point_on_edge, working_object->circle_collider.radius, normal);
								} else {
									new_position = move_point_in_direction(
										working_center, the_circle.radius - distance(the_circle.center, point_on_edge), normal + M_PI
									);
								}
							} else {
								printf("in\n");
							}
						}
					}
					if(working_object->collider == type_circle) {
						if(approx_equals(working_object->circle_collider.center, new_position)) {
							collisions_processed = true;
						} else {
							printf("(%f, %f), (%f, %f)\n", working_object->circle_collider.center.x, working_object->circle_collider.center.x, new_position.x, new_position.y);
							working_object->circle_collider.center.x = new_position.x;
							working_object->circle_collider.center.y = new_position.y;
						}
					} else {
						if(approx_equals(working_object->rect_collider.center, new_position)) {
							collisions_processed = true;
						} else {
							printf("(%f, %f), (%f, %f)\n", working_object->rect_collider.center.x, working_object->rect_collider.center.x, new_position.x, new_position.y);
							working_object->rect_collider.center.x = new_position.x;
							working_object->rect_collider.center.y = new_position.y;
						}
					}

					// find how slanted the collided object is
					point grav_attractor_center = get_center(*working_grav_attractor);
					float dist_from_grav_x = working_center.x - grav_attractor_center.x;
					float dist_from_grav_y = working_center.y - grav_attractor_center.y;
					float angle_to_grav_attractor = atan2(dist_from_grav_y, dist_from_grav_x);
					float normal_angle = get_normal_at_external_point(*comparison_object, working_center);
					float surface_slant = get_real_angle_between(normal_angle, angle_to_grav_attractor);
					if(surface_slant < M_PI_4) {
						// check if the collided object should be a supporter
						working_object->supporter = comparison_object->id;
					} else {
						// check if the collision should reduce upward speed
						movement* vert = get_movement_by_id(working_object, GRAVITY_MOVEMENT);
						if(vert->speed > 0) {
							vert->speed *= 1 - (surface_slant/M_PI);
							if(vert->speed < 0) {
								vert->speed = 0;
							}
						}
					}
				}
				// check if it is colliding with supporter
				if(comparison_object->id == working_object->supporter) {
					still_supported = true;
				}
			}
		}
		// Remove supporter if we've left the ground
		if(!still_supported) {
			working_object->supporter = -1;
		}
	}
}