/*
Copyright © 2011-2012 Clint Bellanger and kitano

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class Entity
 *
 * An Entity represents any character in the game - the player, allies, enemies
 * This base class handles logic common to all of these child classes
 */

#include "Animation.h"
#include "Entity.h"
#include "FileParser.h"
#include "MapRenderer.h"
#include "SharedResources.h"
#include "UtilsParsing.h"

#include <iostream>

using namespace std;

Entity::Entity(MapRenderer* _map) : sprites(NULL), activeAnimation(NULL), map(_map) {
}

/**
 * move()
 * Apply speed to the direction faced.
 *
 * @return Returns false if wall collision, otherwise true.
 */
bool Entity::move() {

	if (stats.forced_move_duration > 0) {
		return map->collider.move(stats.pos.x, stats.pos.y, stats.forced_speed.x, stats.forced_speed.y, 1, stats.movement_type);
	}
	if (stats.immobilize_duration > 0) return false;

	int speed_diagonal = stats.dspeed;
	int speed_straight = stats.speed;

	if (stats.slow_duration > 0) {
		speed_diagonal /= 2;
		speed_straight /= 2;
	}
	else if (stats.haste_duration > 0) {
		speed_diagonal *= 2;
		speed_straight *= 2;
	}

	bool full_move = false;

	switch (stats.direction) {
		case 0:
			full_move = map->collider.move(stats.pos.x, stats.pos.y, -1, 1, speed_diagonal, stats.movement_type);
			break;
		case 1:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, -1, 0, speed_straight, stats.movement_type);
			break;
		case 2:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, -1, -1, speed_diagonal, stats.movement_type);
			break;
		case 3:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 0, -1, speed_straight, stats.movement_type);
			break;
		case 4:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, -1, speed_diagonal, stats.movement_type);
			break;
		case 5:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, 0, speed_straight, stats.movement_type);
			break;
		case 6:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, 1, speed_diagonal, stats.movement_type);
			break;
		case 7:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 0, 1, speed_straight, stats.movement_type);
			break;
	}

	return full_move;
}

/**
 * Change direction to face the target map location
 */
int Entity::face(int mapx, int mapy) {
	// inverting Y to convert map coordinates to standard cartesian coordinates
	int dx = mapx - stats.pos.x;
	int dy = stats.pos.y - mapy;

	// avoid div by zero
	if (dx == 0) {
		if (dy > 0) return 3;
		else return 7;
	}

	float slope = ((float)dy)/((float)dx);
	if (0.5 <= slope && slope <= 2.0) {
		if (dy > 0) return 4;
		else return 0;
	}
	if (-0.5 <= slope && slope <= 0.5) {
		if (dx > 0) return 5;
		else return 1;
	}
	if (-2.0 <= slope && slope <= -0.5) {
		if (dx > 0) return 6;
		else return 2;
	}
	if (2.0 <= slope || -2.0 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return stats.direction;
}

/**
 * Load the entity's animation from animation definition file
 */
void Entity::loadAnimations(const string& filename) {

	if (animations.size() > 0) animations.clear();

	FileParser parser;

	if (!parser.open(mods->locate(filename).c_str())) {
		cout << "Error loading animation definition file: " << filename << endl;
		SDL_Quit();
		exit(1);
	}

	string name = "";
	int position = 0;
	int frames = 0;
	int duration = 0;
	Point render_size;
	Point render_offset;
	string type = "";
	string firstAnimation = "";
	int active_frame = 0;

	// Parse the file and on each new section create an animation object from the data parsed previously

	parser.next();
	parser.new_section = false; // do not create the first animation object until parser has parsed first section

	do {
		// create the animation if finished parsing a section
		if (parser.new_section) {
			Animation *a = new Animation();
			a->init(name, render_size, render_offset,  position, frames, duration, type, active_frame);
			animations.push_back(a);
		}

		if (parser.key == "position") {
			position = toInt(parser.val);
		}
		else if (parser.key == "frames") {
			frames = toInt(parser.val);
		}
		else if (parser.key == "duration") {
			int ms_per_frame = toInt(parser.val);

			duration = (int)round((float)ms_per_frame / (1000.0 / (float)FRAMES_PER_SEC));

			// adjust duration according to the entity's animation speed
			duration = (duration * 100) / stats.animationSpeed;

			// TEMP: if an animation is too fast, display one frame per fps anyway
			if (duration < 1) duration=1;
		}
		else if (parser.key == "type")
			type = parser.val;
		else if (parser.key == "render_size_x")
			render_size.x = toInt(parser.val);
		else if (parser.key == "render_size_y")
			render_size.y = toInt(parser.val);
		else if (parser.key == "render_offset_x")
			render_offset.x = toInt(parser.val);
		else if (parser.key == "render_offset_y")
			render_offset.y = toInt(parser.val);
		else if (parser.key == "active_frame")
			active_frame = toInt(parser.val);

		if (name == "") {
			// This is the first animation
			firstAnimation = parser.section;
		}
		name = parser.section;
	}
	while (parser.next());

	// add final animation
	Animation *a = new Animation();
	a->init(name, render_size, render_offset, position, frames, duration, type, active_frame);
	animations.push_back(a);


	// set the default animation
	if (firstAnimation != "") {
		setAnimation(firstAnimation);
	}
}

/**
 * Set the entity's current animation by name
*/
bool Entity::setAnimation(const string& animationName) {

	// if the animation is already the requested one do nothing
	if (activeAnimation != NULL && activeAnimation->getName() == animationName) {
		return true;
	}

	// search animations for the requested animation and set the active animation to it if found
	for (vector<Animation*>::iterator it = animations.begin(); it!=animations.end(); it++) {
		if ((*it) != NULL && (*it)->getName() == animationName) {
			activeAnimation = *it;
			activeAnimation->reset();
			return true;
		}
	}

	return false;
}

Entity::~Entity () {

	// delete all loaded animations
	for (vector<Animation*>::const_iterator it = animations.begin(); it != animations.end(); it++)
	{
		delete *it;
	}
	animations.clear();
}

