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
 * MenuHPMP
 *
 * Handles the display of the HP and MP bars at the top/left of the screen
 */

#ifndef MENU_HP_MP_H
#define MENU_HP_MP_H

#include "ScopedPtr.h"
#include "SmartSurface.h"

class StatBlock;
class WidgetLabel;
class Point;

class MenuHPMP {
private:
	SmartSurface background;
	SmartSurface bar_hp;
	SmartSurface bar_mp;
	ScopedPtr<WidgetLabel> hphover;
	ScopedPtr<WidgetLabel> mphover;
	
public:
	MenuHPMP();
	void loadGraphics();
	void render(StatBlock *stats, Point mouse);
};

#endif
