#ifndef __GEO__
#define __GEO__

typedef struct point {
	float x;
	float y;
} point;
point create_point_s(float x, float y);
// point* create_point_h(float x, float y);

typedef struct rect {
	point center;
	float width;
	float height;
	float angle;
} rect;
rect create_rect_s(point center, float width, float height, float angle);
// rect* create_rect_h(point center, float width, float height, float angle);

typedef struct rect_corners {
	point upper_left;
	point upper_right;
	point lower_left;
	point lower_right;
} rect_corners;
rect_corners get_rect_corners(rect r);
rect get_rect_from_corners(rect_corners corners);
rect get_adjacent_rect(rect rect, char direction, float length);

typedef struct circle {
	point center;
	float radius;
} circle;
circle create_circle_s(point center, float radius);

void get_array(rect_corners corners, point* destination_array);

bool approx_equals(point p1, point p2);

float distance(point p1, point p2);

point decompose(float distance, float direction_angle);

point move_point_in_direction(point start, float distance, float direction_angle);

point rotate_point_about(point rotating_point, point about_point, float angle);

bool point_in_rect(point p, rect_corners corners);

point collision_rect_rect(rect r1, rect r2);

bool collision_circle_circle(circle c1, circle c2);

bool collision_circle_rect(circle c, rect_corners corners);

point get_intersection(point a1, point a2, point b1, point b2, bool include_off_segment);

float distance_from_point_to_line(point l1, point l2, point the_point);

float distance_from_point_to_line_directional(point l1, point l2, point the_point);

bool circle_contains_circle(circle containing, circle contained);

bool rect_contains_rect(rect_corners containing, rect_corners contained);

bool circle_contains_rect(circle containing_circle, rect_corners contained_rect);

bool rect_contains_circle(rect_corners containing_rect, circle contained_circle);

float get_real_angle_between(float angle_a, float angle_b);

#endif