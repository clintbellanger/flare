/*
Copyright 2011 Clint Bellanger

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
 * class MenuMiniMap
 */

#ifndef MENU_MINI_MAP_H
#define MENU_MINI_MAP_H

#include <SDL.h>
#include "Utils.h"
#include "MapCollision.h"

class MenuMiniMap {
private:
	SDL_Surface *screen;
	Uint32 color_wall;
	Uint32 color_obst;
	Uint32 color_hero;
public: 
	MenuMiniMap(SDL_Surface *_screen);
	~MenuMiniMap();

	void render(MapCollision *collider, Point hero_pos, int map_w, int map_h);

};


#endif
