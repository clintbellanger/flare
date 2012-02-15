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
 * class MenuVendor
 */

#ifndef MENU_VENDOR_H
#define MENU_VENDOR_H

#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "MenuItemStorage.h"
#include "WidgetTooltip.h"
#include "StatBlock.h"
#include "WidgetButton.h"
#include "NPC.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>


const int VENDOR_SLOTS = 80;

class MenuVendor {
private:
	ItemManager *items;
	StatBlock *stats;
	WidgetButton *closeButton;

	void loadGraphics();
	SDL_Surface *background;
	MenuItemStorage stock; // items the vendor currently has in stock

public:
	MenuVendor(ItemManager *items, StatBlock *stats);
	~MenuVendor();

	NPC *npc;

	void loadMerchant(const std::string& filename);
	void logic();
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	TooltipData checkTooltip(Point mouse);
	bool full();
	void setInventory();
	void saveInventory();

	bool visible;
	SDL_Rect slots_area;
};


#endif
