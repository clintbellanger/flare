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

#ifndef ANIMATION_H
#define ANIMATION_H


#include "Utils.h"

#include <string>


struct SDL_Surface;


class Animation {
public:
	Animation(std::string, Point _render_size, Point _render_offset, int position, int frames, int duration, std::string, int _active_frame);

	// advance the animation one frame
	void advanceFrame();
	// return the Renderable of the current frame
	Renderable getCurrentFrame(int direction) const;

	int getCurFrame() const { return cur_frame; }
	int getMaxFrame() const { return max_frame; }
	
	bool isFirstFrame() const { return cur_frame == 1; }
	bool isMiddleFrame() const { return cur_frame == mid_frame; }
	bool isLastFrame() const { return cur_frame == (max_frame -1); }
	
	bool isActiveFrame() const { return cur_frame == active_frame; }
	
	// in a looped animation returns how many times it's been played
	// in a play once animation returns 1 when the animation is finished
	int getTimesPlayed() { return timesPlayed; }

	// resets to beginning of the animation
	void reset();

	std::string getName() const { return name; }

protected:
	std::string name;

	// The sprite sheet
	SDL_Surface* sprites;

	// Animation data
	Point render_size;
	Point render_offset;
	int position;
	int frames;
	int duration;
	std::string type; // eg. play_once or looped
	int cur_frame;
	int disp_frame;
	int mid_frame;
	int max_frame;
	int timesPlayed;
	int active_frame;
};

#endif

