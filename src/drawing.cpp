#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <fstream>
#include "drawing.h"
#include "geometry.h"
#include "physics.h"
#include "editing.h"

point reverse_camera_transform_point(rect camera_rect, point screen_point) {
	float scale = camera_rect.width/(float)SCREEN_WIDTH;
	screen_point = rotate_point_about(screen_point, camera_rect.center, camera_rect.angle);
	screen_point.x = screen_point.x + camera_rect.center.x/scale - SCREEN_WIDTH/2;
	screen_point.y = screen_point.y + camera_rect.center.y/scale - SCREEN_HEIGHT/2;
	screen_point.x *= scale;
	screen_point.y *= scale;
	return screen_point;
}

rect camera_transform_rect(rect camera_rect, rect target_rect) {
	float scale = (float)SCREEN_WIDTH/camera_rect.width;
	target_rect.width *= scale;
	target_rect.height *= scale;
	target_rect.center.x *= scale;
	target_rect.center.y *= scale;
	target_rect.center.x = target_rect.center.x - camera_rect.center.x*scale + SCREEN_WIDTH/2;
	target_rect.center.y = target_rect.center.y - camera_rect.center.y*scale + SCREEN_HEIGHT/2;
	return get_rect_from_corners(rotate_corners_about(get_rect_corners(target_rect), camera_rect.center, - camera_rect.angle));
}

circle camera_transform_circle(rect camera_rect, circle target_circle) {
	float scale = (float)SCREEN_WIDTH/camera_rect.width;
	target_circle.radius *= scale;
	target_circle.center.x *= scale;
	target_circle.center.y *= scale;
	target_circle.center.x = target_circle.center.x - camera_rect.center.x*scale + SCREEN_WIDTH/2;
	target_circle.center.y = target_circle.center.y - camera_rect.center.y*scale + SCREEN_HEIGHT/2;
	target_circle.center = rotate_point_about(target_circle.center, camera_rect.center, - camera_rect.angle);
	return target_circle;
}

sprite_animation load_animation_from_disk(SDL_Renderer* renderer, std::string name) {
	sprite_animation anim;
	std::ifstream fs ("assets/"+name+".txt");

	std::string first_line;
	getline(fs, first_line);
	char c_first_line[first_line.size()];
	strcpy(c_first_line, first_line.c_str());
	char* seconds = strtok(c_first_line, ",");
	anim.total_seconds = std::stof(seconds);
	char* width = strtok(NULL, ",");
	anim.src_width = std::stoi(width);

	std::vector<float> frame_timing;
	std::string line;
	float working_time = 0;
	while(getline(fs, line)) {
		working_time += std::stof(line)*anim.total_seconds;
		frame_timing.push_back(working_time);
	}
	anim.frame_timing = frame_timing;

	fs.close();
	SDL_Surface* img = IMG_Load("assets/anim.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
	anim.texture = texture;
	anim.src_height = img->h;
	SDL_FreeSurface(img);

	anim.elapsed_seconds = 0;
	return anim;
}

rect get_animation_rect(sprite_animation anim, float size, char dimension) {
	rect r;
	r.angle = 0;
	float ratio = (float)anim.src_width/(float)anim.src_height;
	if(dimension == 'w') {
		r.width = size;
		r.height = size/ratio;
	} else if(dimension == 'h') {
		r.height = size;
		r.width = size*ratio;
	} else {
		r.height = size;
		r.width = size;
	}
	return r;
}

void draw_sprite_anim(SDL_Renderer* renderer, rect camera_rect, sprite_animation* anim, rect target, float elapsed_time_ms) {
	// draw_rectangle(renderer, camera_rect, target, true);
	target = camera_transform_rect(camera_rect, target);
	anim->elapsed_seconds += elapsed_time_ms/1000;
	while(anim->elapsed_seconds > anim->total_seconds) {
		anim->elapsed_seconds -= anim->total_seconds;
	}
	int frame_count = anim->frame_timing.size();
	for(int i=0; i<frame_count; i++) {
		if(anim->frame_timing[i] > anim->elapsed_seconds) {
			SDL_Rect src;
			src.w = anim->src_width;
			src.h = anim->src_height;
			src.x = anim->src_width * i;
			src.y = 0;
			SDL_Rect dst;
			dst.w = target.width;
			dst.h = target.height;
			dst.x = target.center.x - target.width/2;
			dst.y = target.center.y - target.height/2;
			SDL_Point pt;
			pt.x = target.center.x;
			pt.y = target.center.y;
			SDL_RenderCopyEx(renderer, anim->texture, &src, &dst, rad_to_deg(target.angle), NULL, SDL_FLIP_NONE);
			break;
		}
	}
}

void draw_rectangle(SDL_Renderer* renderer, rect camera_rect, rect r, bool highlight_top) {
	r = camera_transform_rect(camera_rect, r);
	rect_corners corners = get_rect_corners(r);

	// SDL_Point points[5] = {
	// 	{corners.upper_left.x, corners.upper_left.y},
	// 	{corners.upper_right.x, corners.upper_right.y},
	// 	{corners.lower_right.x, corners.lower_right.y},
	// 	{corners.lower_left.x, corners.lower_left.y},
	// 	{corners.upper_left.x, corners.upper_left.y}
	// };
	// SDL_RenderDrawLines(renderer, points, 5);

	// normals
	// float normal_count = 120;
	// for(int i=0; i<normal_count; i++) {
	// 	float angle = (M_PI * 2 * i) / normal_count;
	// 	point external = move_point_in_direction(r.center, std::max(r.width, r.height), angle);
	// 	draw_circle(renderer, camera_rect, create_circle_s(external, 3));
	// 	float normal = get_normal_for_rectangle_by_direction(r, get_containing_adjacent_rect_for_point(r, external), external);
	// 	point normal_moved_pt = move_point_in_direction(external, 15, normal);
	// 	SDL_RenderDrawLine(renderer, external.x, external.y, normal_moved_pt.x, normal_moved_pt.y);
	// }

	// non-top
	SDL_RenderDrawLine(renderer, corners.lower_left.x, corners.lower_left.y, corners.lower_right.x, corners.lower_right.y);
	SDL_RenderDrawLine(renderer, corners.upper_left.x, corners.upper_left.y, corners.lower_left.x, corners.lower_left.y);
	SDL_RenderDrawLine(renderer, corners.upper_right.x, corners.upper_right.y, corners.lower_right.x, corners.lower_right.y);
	// top
	if(highlight_top) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
	}
	SDL_RenderDrawLine(renderer, corners.upper_left.x, corners.upper_left.y, corners.upper_right.x, corners.upper_right.y);
}

void draw_circle(SDL_Renderer* renderer, rect camera_rect, circle c) {
	c = camera_transform_circle(camera_rect, c);
	int vertices_count = 30;
	float angle_size = (2*M_PI)/(vertices_count - 1);

	point p = create_point_s(c.center.x, c.center.y + c.radius);
	SDL_Point sdl_points[vertices_count];
	for(int i=0; i<vertices_count; i++) {
		p = rotate_point_about(p, c.center, angle_size);
		SDL_Point sdl_point;
		sdl_point.x = p.x;
		sdl_point.y = p.y;
		sdl_points[i] = sdl_point;
	}
	SDL_RenderDrawLines(renderer, sdl_points, vertices_count);
}

void draw_intersecting_segments(SDL_Renderer* renderer, point a1, point a2, point b1, point b2) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, a1.x, a1.y, a2.x, a2.y);
	SDL_RenderDrawLine(renderer, b1.x, b1.y, b2.x, b2.y);
	point sectoff = get_intersection(a1, a2, b1, b2, true);
	point sect = get_intersection(a1, a2, b1, b2, false);
	if(!isnan(sectoff.x)) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_Rect sdlrect = {sectoff.x, sectoff.y, 5, 5};
		SDL_RenderFillRect(renderer, &sdlrect);
	}
	if(!isnan(sect.x)) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_Rect sdlrect = {sect.x, sect.y, 5, 5};
		SDL_RenderFillRect(renderer, &sdlrect);
	}
}

// void draw_point_in_rect(SDL_Renderer* renderer, rect r, point p) {
// 	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
// 	draw_rectangle(renderer, r);
// 	if(point_in_rect(p, get_rect_corners(r))) {
// 		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
// 	} else {
// 		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
// 	}

// 	SDL_Rect sdlrect = {p.x, p.y, 5, 5};
// 	SDL_RenderFillRect(renderer, &sdlrect);
// }

// void draw_distance_from_point_to_line(SDL_Renderer* renderer, point p, point pl1, point pl2) {
// 	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
// 	SDL_Rect sdlrect = {p.x, p.y, 5, 5};
// 	SDL_RenderFillRect(renderer, &sdlrect);

// 	float dist = distance_from_point_to_line(pl1, pl2, p);
// 	circle c = create_circle_s(p, dist);

// 	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
// 	draw_circle(renderer, c);

// 	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
// 	SDL_RenderDrawLine(renderer, pl1.x, pl1.y, pl2.x, pl2.y);
// }

void draw_normal_from_point(SDL_Renderer* renderer, physics_object obj, point p) {
	float angle = get_normal_at_external_point(obj, p);
	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
	SDL_Rect sdlrect = {p.x, p.y, 5, 5};
	SDL_RenderFillRect(renderer, &sdlrect);
	point p2 = move_point_in_direction(p, 50, angle);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, p.x, p.y, p2.x, p2.y);
}

void draw_physics_objects_play_mode(SDL_Renderer* renderer, rect camera_rect, std::vector<physics_object> physics) {
	for(int i=0; i<physics.size(); i++) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		if(physics[i].collided.size() > 0) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		}
		if(physics[i].collider == type_circle) {
			draw_circle(renderer, camera_rect, physics[i].circle_collider);
		} else {
			draw_rectangle(renderer, camera_rect, physics[i].rect_collider, true);
		}
	}
}

void draw_physics_objects_edit_mode(SDL_Renderer* renderer, rect camera_rect, std::vector<physics_object> physics, editor_state ed) {
	for(int i=0; i<physics.size(); i++) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		for(int j=0; j<ed.selected_objects.size(); j++) {
			if(ed.selected_objects[j]->id == physics[i].id) {
				SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			}
		}
		if(physics[i].collider == type_circle) {
			draw_circle(renderer, camera_rect, physics[i].circle_collider);
		} else {
			draw_rectangle(renderer, camera_rect, physics[i].rect_collider, false);
		}
	}
}