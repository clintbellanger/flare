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
 * class MenuItemStorage
 */

#ifndef MENU_ITEM_STORAGE_H
#define MENU_ITEM_STORAGE_H

#include "ItemManager.h"
#include "ItemStorage.h"
#include <SDL.h>

class InputState;
class TooltipData;

class MenuItemStorage : public ItemStorage {
protected:
	std::vector<SDL_Rect> area;
	int icon_size;
	int nb_cols;
	std::vector<std::string> slot_type;

public:
	void init(int _slot_number, ItemManager *_items, SDL_Rect _area, int icon_size, int nb_cols);
	void init(int _slot_number, ItemManager *_items, std::vector<SDL_Rect> _area, int icon_size, std::vector<std::string> _slot_type);

	// rendering
	void render();
	int slotOver(Point mouse);
	TooltipData checkTooltip(Point mouse, StatBlock *stats, bool vendor_view);
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);

	int drag_prev_slot;
};

#endif


