/*
Copyright © 2011-2012 kitano

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
 * class Animation
 *
 * The Animation class handles the logic of advancing frames based on the animation type
 * and returning a renderable frame.
 *
 * The intention with the class is to keep it as flexible as possible so that the animations
 * can be used not only for character animations but any animated in-game objects.
 */

#include "Animation.h"

Animation::Animation(std::string _name, Point _render_size, Point _render_offset, int _position, int _frames, int _duration, std::string _type, int /*_active_frame*/)
	: name(_name), sprites(NULL),
	  render_size(_render_size), render_offset(_render_offset),
	  position(_position), frames(_frames), duration(_duration), type(_type),
	  cur_frame(0), disp_frame(0), mid_frame(0), max_frame(0), timesPlayed(0), active_frame(0) {

	if (type == "play_once" || type == "looped") {
		max_frame = frames * duration;
	}
	else if (type == "back_forth") {
		mid_frame = frames * duration;
		max_frame = mid_frame + mid_frame;

	}
}

void Animation::advanceFrame() {

	if (type == "play_once") {
		if (cur_frame < max_frame - 1) {
			cur_frame++;
		}
		else {
			timesPlayed = 1;
		}
		disp_frame = (cur_frame / duration) + position;
	}
	else if (type == "looped") {
		cur_frame++;
		if (cur_frame == max_frame) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
		disp_frame = (cur_frame / duration) + position;

	}
	else if (type == "back_forth") {
		cur_frame++;

		if (cur_frame == max_frame) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}

		if (cur_frame >= mid_frame) {
			disp_frame = (max_frame -1 - cur_frame) / duration + position;
		}
		else {
			disp_frame = cur_frame / duration + position;
		}

	}
}

Renderable Animation::getCurrentFrame(int direction) {
	Renderable r;

	// if the animation contains the spritesheet
	if (sprites != NULL) {
		r.sprite = sprites;
	}

	r.src.x = render_size.x * disp_frame;
	r.src.y = render_size.y * direction;
	r.src.w = render_size.x;
	r.src.h = render_size.y;
	r.offset.x = render_offset.x;
	r.offset.y = render_offset.y; // 112
	return r;
}

void Animation::reset() {
	cur_frame = 0;
	disp_frame = (cur_frame / duration) + position;
	timesPlayed = 0;
}

