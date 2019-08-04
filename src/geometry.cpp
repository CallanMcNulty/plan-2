#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "globals.h"
#include "geometry.h"

point create_point_s(float x, float y) {
	point p;
	p.x = x;
	p.y = y;
	return p;
}
// point* create_point_h(float x, float y) {
// 	point* point_mem = (point*)malloc(sizeof(point));
// 	point_mem->x = x;
// 	point_mem->y = y;
// 	return point_mem;
// }


rect create_rect_s(point center, float width, float height, float angle) {
	rect r;
	r.center = center;
	r.width = width;
	r.height = height;
	r.angle = angle;
	return r;
}
// rect* create_rect_h(point center, float width, float height, float angle) {
// 	rect* rect_mem = (rect*)malloc(sizeof(rect));
// 	rect_mem->center = center;
// 	rect_mem->width = width;
// 	rect_mem->height = height;
// 	rect_mem->angle = angle;
// 	return rect_mem;
// }
rect_corners get_rect_corners(rect r) {
	float upper_left_x = r.center.x - r.width/2;
	float upper_left_y = r.center.y - r.height/2;
	float upper_right_x = r.center.x + r.width/2;
	float upper_right_y = r.center.y - r.height/2;
	float lower_left_x = r.center.x - r.width/2;
	float lower_left_y = r.center.y + r.height/2;
	float lower_right_x = r.center.x + r.width/2;
	float lower_right_y = r.center.y + r.height/2;
	point upper_left = rotate_point_about(create_point_s(upper_left_x, upper_left_y), r.center, r.angle);
	point upper_right = rotate_point_about(create_point_s(upper_right_x, upper_right_y), r.center, r.angle);
	point lower_left = rotate_point_about(create_point_s(lower_left_x, lower_left_y), r.center, r.angle);
	point lower_right = rotate_point_about(create_point_s(lower_right_x, lower_right_y), r.center, r.angle);
	rect_corners corners;
	corners.upper_left = upper_left;
	corners.upper_right = upper_right;
	corners.lower_left = lower_left;
	corners.lower_right = lower_right;
	return corners;
}
rect get_rect_from_corners(rect_corners corners) {
	float angle = atan2(corners.upper_left.y - corners.upper_right.y, corners.upper_left.x - corners.upper_right.x);
	float height = distance(corners.upper_left, corners.lower_left);
	float width = distance(corners.upper_left, corners.upper_right);
	float center_x = (corners.upper_left.x + corners.lower_left.x + corners.upper_right.x + corners.lower_right.x) / 4;
	float center_y = (corners.upper_left.y + corners.lower_left.y + corners.upper_right.y + corners.lower_right.y) / 4;
	return create_rect_s(create_point_s(center_x, center_y), width, height, angle);
}
rect get_adjacent_rect(rect rect, char direction, float length) {
	rect_corners corners = get_rect_corners(rect);
	rect_corners adjacent_corners;
	switch(direction) {
		case('8'): {
			adjacent_corners.lower_left = corners.upper_left;
			adjacent_corners.lower_right = corners.upper_right;
			adjacent_corners.upper_left = move_point_in_direction(corners.upper_left, length, rect.angle - M_PI/2);
			adjacent_corners.upper_right = move_point_in_direction(corners.upper_right, length, rect.angle - M_PI/2);
			break;
		}
		case('2'): {
			adjacent_corners.upper_left = corners.lower_left;
			adjacent_corners.upper_right = corners.lower_right;
			adjacent_corners.lower_left = move_point_in_direction(corners.lower_left, length, rect.angle + M_PI/2);
			adjacent_corners.lower_right = move_point_in_direction(corners.lower_right, length, rect.angle + M_PI/2);
			break;
		}
		case('4'): {
			adjacent_corners.upper_right = corners.upper_left;
			adjacent_corners.lower_right = corners.lower_left;
			adjacent_corners.upper_left = move_point_in_direction(corners.upper_left, length, rect.angle + M_PI);
			adjacent_corners.lower_left = move_point_in_direction(corners.lower_left, length, rect.angle + M_PI);
			break;
		}
		case('6'): {
			adjacent_corners.upper_left = corners.upper_right;
			adjacent_corners.lower_left = corners.lower_right;
			adjacent_corners.upper_right = move_point_in_direction(corners.upper_right, length, rect.angle);
			adjacent_corners.lower_right = move_point_in_direction(corners.lower_right, length, rect.angle);
			break;
		}
		case('7'): {
			adjacent_corners.lower_right = corners.upper_left;
			adjacent_corners.lower_left = move_point_in_direction(corners.upper_left, length, rect.angle + M_PI);
			adjacent_corners.upper_right = move_point_in_direction(corners.upper_left, length, rect.angle - M_PI/2);
			adjacent_corners.upper_left = move_point_in_direction(adjacent_corners.lower_left, length, rect.angle - M_PI/2);
			break;
		}
		case('9'): {
			adjacent_corners.lower_left = corners.upper_right;
			adjacent_corners.lower_right = move_point_in_direction(corners.upper_right, length, rect.angle);
			adjacent_corners.upper_left = move_point_in_direction(corners.upper_right, length, rect.angle - M_PI/2);
			adjacent_corners.upper_right = move_point_in_direction(adjacent_corners.lower_right, length, rect.angle - M_PI/2);
			break;
		}
		case('3'): {
			adjacent_corners.upper_left = corners.lower_right;
			adjacent_corners.upper_right = move_point_in_direction(corners.lower_right, length, rect.angle);
			adjacent_corners.lower_left = move_point_in_direction(corners.lower_right, length, rect.angle + M_PI/2);
			adjacent_corners.lower_right = move_point_in_direction(adjacent_corners.upper_right, length, rect.angle + M_PI/2);
			break;
		}
		case('1'): {
			adjacent_corners.upper_right = corners.lower_left;
			adjacent_corners.upper_left = move_point_in_direction(corners.lower_left, length, rect.angle + M_PI);
			adjacent_corners.lower_right = move_point_in_direction(corners.lower_left, length, rect.angle + M_PI/2);
			adjacent_corners.lower_left = move_point_in_direction(adjacent_corners.upper_left, length, rect.angle + M_PI/2);
			break;
		}
	}
	return get_rect_from_corners(adjacent_corners);
}
void get_array(rect_corners corners, point* destination_array) {
	destination_array[0] = corners.upper_left;
	destination_array[1] = corners.upper_right;
	destination_array[2] = corners.lower_left;
	destination_array[3] = corners.lower_right;
}

circle create_circle_s(point center, float radius) {
	circle c;
	c.center = center;
	c.radius = radius;
	return c;
}

bool approx_equals(point p1, point p2) {
	return abs(p1.x - p2.x) < .0001 && abs(p1.y - p2.y) < .0001;
}

float distance(point p1, point p2) {
	return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

// return point is the delta x and delta y of the vector, not a point, per se
point decompose(float distance, float direction_angle) {
	return create_point_s(distance * cos(direction_angle), distance * sin(direction_angle));
}

point move_point_in_direction(point start, float distance, float direction_angle) {
	point delta = decompose(distance, direction_angle);
	return create_point_s(start.x + delta.x, start.y + delta.y);
}

point rotate_point_about(point rotating_point, point about_point, float angle) {
	float temp_x = rotating_point.x - about_point.x;
	float temp_y = rotating_point.y - about_point.y;
	float rotated_x = temp_x*cos(angle) - temp_y*sin(angle);
	float rotated_y = temp_x*sin(angle) + temp_y*cos(angle);
	return create_point_s(rotated_x + about_point.x, rotated_y + about_point.y);
}

// returns NAN is there is no intersection, otherwise returns x coord of intersection
float segment_intersects_with_horizontal(point p1, point p2, float horiz_y) {
	bool intersection_exists = (std::max(p1.y, p2.y) > horiz_y && std::min(p1.y, p2.y) < horiz_y) || p1.y == horiz_y;
	if(!intersection_exists) {
		return NAN;
	}
	float slope = (p2.y - p1.y)/(p2.x - p1.x);
	// float x_dist_to_intersection = (p2.y - p1.y)/slope;
	// return p1.x + x_dist_to_intersection;
	return ((horiz_y - p1.y) / slope) + p1.x;
}

bool point_in_rect(point p, rect_corners corners) {
	int intersection_count = 0;
	float intersects_with_top_x = segment_intersects_with_horizontal(corners.upper_left, corners.upper_right, p.y);
	// if the point is on the line, that's an automatic intersection
	if(approx(intersects_with_top_x, p.x)) {
		return true;
	}
	if(!isnan(intersects_with_top_x) && intersects_with_top_x >= p.x) {
		intersection_count ++;
	}

	float intersects_with_left_x = segment_intersects_with_horizontal(corners.lower_left, corners.upper_left, p.y);
	if(approx(intersects_with_left_x, p.x)) {
		return true;
	}
	if(!isnan(intersects_with_left_x) && intersects_with_left_x >= p.x) {
		intersection_count ++;
	}

	float intersects_with_bottom_x = segment_intersects_with_horizontal(corners.lower_right, corners.lower_left, p.y);
	if(approx(intersects_with_bottom_x, p.x)) {
		return true;
	}
	if(!isnan(intersects_with_bottom_x) && intersects_with_bottom_x >= p.x) {
		intersection_count ++;
	}

	float intersects_with_right_x = segment_intersects_with_horizontal(corners.upper_right, corners.lower_right, p.y);
	if(approx(intersects_with_right_x, p.x)) {
		return true;
	}
	if(!isnan(intersects_with_right_x) && intersects_with_right_x >= p.x) {
		intersection_count ++;
	}

	return intersection_count % 2 > 0;
}

point collision_rect_rect(rect r1, rect r2) {
	point c1_array[4];
	rect_corners c1 = get_rect_corners(r1);
	get_array(c1, c1_array);
	point c2_array[4];
	rect_corners c2 = get_rect_corners(r2);
	get_array(c2, c2_array);
	for(int i=0; i<4; i++) {
		if(point_in_rect(c1_array[i], c2)) {
			return c1_array[i];
		} else if (point_in_rect(c2_array[i], c1)) {
			return c2_array[i];
		}
	}
	return create_point_s(NAN, NAN);
}

bool collision_circle_circle(circle c1, circle c2) {
	float sum_of_radii = c1.radius+c2.radius;
	float dist = distance(c1.center, c2.center);
	return sum_of_radii >= dist || approx(sum_of_radii, dist);
}

bool collision_circle_rect(circle c, rect_corners corners) {
	int in_range_count = 0;
	int inside_count = 0;
	float top_dist = distance_from_point_to_line_directional(corners.upper_left, corners.upper_right, c.center);
	if(approx_lt(top_dist, c.radius)) {
		in_range_count ++;
		if(approx_lt(distance(corners.upper_left, c.center), c.radius)) {
			return true;
		}
	}
	if(approx_lt(top_dist, 0)) {
		inside_count ++;
	}
	float right_dist = distance_from_point_to_line_directional(corners.upper_right, corners.lower_right, c.center);
	if(approx_lt(right_dist, c.radius)) {
		in_range_count ++;
		if(approx_lt(distance(corners.upper_right, c.center), c.radius)) {
			return true;
		}
	}
	if(approx_lt(right_dist, 0)) {
		inside_count ++;
	}
	float bottom_dist = distance_from_point_to_line_directional(corners.lower_right, corners.lower_left, c.center);
	if(approx_lt(bottom_dist, c.radius)) {
		in_range_count ++;
		if(approx_lt(distance(corners.lower_right, c.center), c.radius)) {
			return true;
		}
	}
	if(approx_lt(bottom_dist, 0)) {
		inside_count ++;
	}
	float left_dist = distance_from_point_to_line_directional(corners.lower_left, corners.upper_left, c.center);
	if(approx_lt(left_dist, c.radius)) {
		in_range_count ++;
		if(approx_lt(distance(corners.lower_left, c.center), c.radius)) {
			return true;
		}
	}
	if(approx_lt(left_dist, 0)) {
		inside_count ++;
	}
	return in_range_count == 4 && inside_count >= 3;
}

// returns (NAN, NAN) if there is no intersection
point get_intersection(point a1, point a2, point b1, point b2, bool include_off_segment) {
	float delta_x_a = a2.x - a1.x;
	float delta_x_b = b2.x - b1.x;
	float slope_a = (a2.y - a1.y) / delta_x_a;
	float slope_b = (b2.y - b1.y) / delta_x_b;

	// parallel
	if(slope_a == slope_b) {
		return create_point_s(NAN, NAN);
	}

	// handle when one of the lines is vertical (has no slope)
	if(delta_x_a == 0) {
		return create_point_s(a1.x, ((b2.y - b1.y)*(a1.x - b1.x))/(b2.x - b1.x) + b1.y);
	}
	if(delta_x_b == 0) {
		return create_point_s(b1.x, ((a2.y - a1.y)*(b1.x - a1.x))/(a2.x - a1.x) + a1.y);
	}

	// intersection
	float intercept_a = a1.y - (slope_a * a1.x);
	float intercept_b = b1.y - (slope_b * b1.x);
	float intersection_x = (intercept_b - intercept_a) / (slope_a - slope_b);
	float intersection_y = slope_a * intersection_x + intercept_a;

	// off-segment
	if(
		!include_off_segment && (
		approx_lt(intersection_x, std::min(a1.x, a2.x)) ||
		approx_lt(intersection_x, std::min(b1.x, b2.x)) ||
		approx_lt(intersection_y, std::min(a1.y, a2.y)) ||
		approx_lt(intersection_y, std::min(b1.y, b2.y)) ||
		approx_lt(intersection_x, std::max(a1.x, a2.x)) ||
		approx_lt(intersection_x, std::max(b1.x, b2.x)) ||
		approx_lt(intersection_y, std::max(a1.y, a2.y)) ||
		approx_lt(intersection_y, std::max(b1.y, b2.y)) )
	) {
		return create_point_s(NAN, NAN);
	}

	// return intersection
	return create_point_s(intersection_x, intersection_y);
}

float distance_from_point_to_line(point l1, point l2, point the_point) {
	return abs((l2.y - l1.y)*the_point.x - (l2.x - l1.x)*the_point.y + l2.x*l1.y - l2.y*l1.x) / distance(l1, l2);
}

float distance_from_point_to_line_directional(point l1, point l2, point the_point) {
	return ((l2.y - l1.y)*the_point.x - (l2.x - l1.x)*the_point.y + l2.x*l1.y - l2.y*l1.x) / distance(l1, l2);
}

bool circle_contains_circle(circle containing, circle contained) {
	return distance(containing.center, contained.center) + contained.radius < containing.radius;
}

bool rect_contains_rect(rect_corners containing, rect_corners contained) {
	return  (
		point_in_rect(contained.lower_left, containing) &&
		point_in_rect(contained.lower_right, containing) &&
		point_in_rect(contained.upper_left, containing) &&
		point_in_rect(contained.upper_right, containing)
	);
}

bool circle_contains_rect(circle containing_circle, rect_corners contained_rect) {
	point corners_array[4];
	get_array(contained_rect, corners_array);
	for(int i=0; i<4; i++) {
		if(distance(corners_array[i], containing_circle.center) > containing_circle.radius) {
			return false;
		}
	}
	return true;
}

bool rect_contains_circle(rect_corners containing_rect, circle contained_circle) {
	int in_range_count = 0;
	float top_dist = distance_from_point_to_line_directional(
		containing_rect.upper_left, containing_rect.upper_right, contained_circle.center
	);
	if(top_dist < contained_circle.radius * -1) {
		in_range_count ++;
	}
	float right_dist = distance_from_point_to_line_directional(
		containing_rect.upper_right, containing_rect.lower_right, contained_circle.center
	);
	if(right_dist < contained_circle.radius * -1) {
		in_range_count ++;
	}
	float bottom_dist = distance_from_point_to_line_directional(
		containing_rect.lower_right, containing_rect.lower_left, contained_circle.center
	);
	if(bottom_dist < contained_circle.radius * -1) {
		in_range_count ++;
	}
	float left_dist = distance_from_point_to_line_directional(
		containing_rect.lower_left, containing_rect.upper_left, contained_circle.center
	);
	if(left_dist < contained_circle.radius * -1) {
		in_range_count ++;
	}
	return in_range_count == 4;
}

float get_real_angle_between(float angle_a, float angle_b) {
	// float angle_a = atan2(a2.y - a1.y, a2.x - a1.x);
	// float angle_b = atan2(b2.y - b1.y, b2.x - b1.x);
	float diff = abs(angle_b - angle_a);
	while(diff > M_PI*2) {
		diff -= M_PI*2;
	}
	if(diff > M_PI) {
		diff = M_PI*2 - diff;
	}
	return diff;
}