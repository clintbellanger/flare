/*
Copyright © 2011-2012 Clint Bellanger

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
 * MenuExperience
 *
 * Handles the display of the Experience bar on the HUD
 */

#ifndef MENU_EXPERIENCE_H
#define MENU_EXPERIENCE_H

#include "StatBlock.h"
#include "Utils.h"
#include "FontEngine.h"

#include <SDL.h>
#include <SDL_image.h>


class MenuExperience {
private:
	SDL_Surface *background;
	SDL_Surface *bar;
public:
	MenuExperience();
	~MenuExperience();
	void loadGraphics();
	void render(StatBlock *stats, Point mouse);
	
	// variables to make the experience bar display configurable
	
	// hud_position is the bounding box for the entire menu.
	// This is useful for checking mouseover.
	// Also, changing hud_position (x,y) should move the frame contents properly
	SDL_Rect hud_position;
	
	Point background_offset; // offset from hud_position (x,y)
	Point background_size;
	Point bar_offset; // offset from hud_position (x,y)
	Point bar_size;
	Point text_offset;
	int text_justify;
};

#endif
