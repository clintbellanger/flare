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
 * class MenuVendor
 */

#ifndef MENU_VENDOR_H
#define MENU_VENDOR_H

#include "MenuItemStorage.h"
#include "ScopedPtr.h"
#include "SmartSurface.h"

#include <SDL.h>

#include <string>

class ItemManager;
class NPC;
class StatBlock;
class TooltipData;
class WidgetButton;

const int VENDOR_SLOTS = 80;

class MenuVendor {
private:
	ItemManager *items;
	StatBlock *stats;
	ScopedPtr<WidgetButton> closeButton;

	void loadGraphics();
	SmartSurface background;
	MenuItemStorage stock; // items the vendor currently has in stock

public:
	MenuVendor(ItemManager *items, StatBlock *stats);

	NPC *npc;

	void loadMerchant(const std::string& filename);
	void logic();
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	void checkTooltip(Point mouse, TooltipData& tip);
	bool full();
	void setInventory();
	void saveInventory();

	bool visible;
	SDL_Rect slots_area;
};


#endif
