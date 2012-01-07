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

#include "MenuHPMP.h"
#include "FontEngine.h"
#include "ModManager.h"
#include "SharedResources.h"
#include "StatBlock.h"
#include "WidgetLabel.h"

#include <SDL_image.h>

#include <string>
#include <sstream>


using namespace std;


MenuHPMP::MenuHPMP() {
	
	hphover.reset(new WidgetLabel());
	mphover.reset(new WidgetLabel());
	hphover->set(53, 9, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);
	mphover->set(53, 24, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);

	loadGraphics();
}

void MenuHPMP::loadGraphics() {

	background.reset_and_load("images/menus/bar_hp_mp.png");
	bar_hp.reset_and_load("images/menus/bar_hp.png");
	bar_mp.reset_and_load("images/menus/bar_mp.png");
	
	background.display_format_alpha();
	bar_hp.display_format_alpha();
	bar_mp.display_format_alpha();
}

void MenuHPMP::render(StatBlock *stats, Point mouse) {
	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	int mp_bar_length;
	
	// draw trim/background
	src.x = dest.x = 0;
	src.y = dest.y = 0;
	src.w = dest.w = 106;
	src.h = dest.h = 33;
	
	SDL_BlitSurface(background.get(), &src, screen, &dest);
	
	if (stats->maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (stats->hp * 100) / stats->maxhp;
		
	if (stats->maxmp == 0)
		mp_bar_length = 0;
	else
		mp_bar_length = (stats->mp * 100) / stats->maxmp;
	
	// draw hp bar
	src.x = 0;
	src.y = 0;
	src.h = 12;
	dest.x = 3;
	dest.y = 3;
	src.w = hp_bar_length;	
	SDL_BlitSurface(bar_hp.get(), &src, screen, &dest);
	
	// draw mp bar
	dest.y = 18;
	src.w = mp_bar_length;
	SDL_BlitSurface(bar_mp.get(), &src, screen, &dest);		
	
	// if mouseover, draw text
	if (mouse.x <= 106 && mouse.y <= 33) {

		stringstream ss;
		ss << stats->hp << "/" << stats->maxhp;
		hphover->set(ss.str());
		hphover->render();

		ss.str("");
		ss << stats->mp << "/" << stats->maxmp;
		mphover->set(ss.str());
		mphover->render();
	}
}

