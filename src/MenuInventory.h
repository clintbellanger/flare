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
 * class MenuInventory
 */

#ifndef MENU_INVENTORY_H
#define MENU_INVENTORY_H

#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "ItemManager.h"
#include "WidgetTooltip.h"
#include "StatBlock.h"
#include "PowerManager.h"
#include "MenuItemStorage.h"
#include "WidgetButton.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

const int EQUIPMENT = 0;
const int CARRIED = 1;

const int MAX_EQUIPPED = 4;
const int MAX_CARRIED = 64;

enum InventorySlotsType {
	SLOT_MAIN = 0,
	SLOT_BODY = 1,
	SLOT_OFF = 2,
	SLOT_ARTIFACT = 3
};


class MenuInventory : private Uncopyable {
private:
	ItemManager &items;
	StatBlock &stats;
	PowerManager &powers;

	void loadGraphics();
	int areaOver(const Point &mouse);
	void updateEquipment(int slot);

	SDL_Surface *background;
	WidgetButton *closeButton;
	
public:
	MenuInventory(ItemManager &items, StatBlock &stats, PowerManager &powers);
	~MenuInventory();
	void logic();
	void render();
	TooltipData checkTooltip(const Point &mouse);

	ItemStack click(const InputState &input);
	void itemReturn(const ItemStack &stack);
	void drop(const Point &mouse, const ItemStack &stack);
	void activate(const InputState &input);

	void add(const ItemStack &stack, int area = CARRIED, int slot = -1);
	void remove(const Item &item);
	void addGold(int count);
	bool buy(const ItemStack &stack, const Point &mouse = Point());
	bool sell(const ItemStack &stack);

	bool full();
	int getItemCountCarried(const Item &item) const;
	bool isItemEquipped(const Item &item) const;
	bool requirementsMet(const Item &item) const;
	
	void applyEquipment(ItemStack *equipped);

	const Item *getMainHand() const		{return inventory[EQUIPMENT][SLOT_MAIN].item;}
	const Item *getArmor() const		{return inventory[EQUIPMENT][SLOT_BODY].item;}
	const Item *getOffHand() const		{return inventory[EQUIPMENT][SLOT_OFF].item;}
	const Item *getArtifact() const		{return inventory[EQUIPMENT][SLOT_ARTIFACT].item;}

	bool visible;

	SDL_Rect window_area;
	SDL_Rect carried_area;
	SDL_Rect equipped_area;

	MenuItemStorage inventory[2];
	int gold;
	int drag_prev_src;

	// the following two are separate because artifacts don't display on the hero.
	// so we only update the hero sprites when non-artifact changes occur.
	bool changed_equipment;
	bool changed_artifact;
	
	std::string log_msg;

};

#endif

