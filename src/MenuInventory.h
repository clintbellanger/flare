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
 * class MenuInventory
 */

#ifndef MENU_INVENTORY_H
#define MENU_INVENTORY_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "ItemDatabase.h"
#include "MenuTooltip.h"
#include "StatBlock.h"
#include "PowerManager.h"
#include "MenuItemStorage.h"
#include "WidgetButton.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

using namespace std;

const int EQUIPMENT = 0;
const int CARRIED = 1;

const int MAX_EQUIPPED = 4;
const int MAX_CARRIED = 64;

// note: if you change these, also change them in ItemDatabase::applyEquipment
const int SLOT_MAIN = 0;
const int SLOT_BODY = 1;
const int SLOT_OFF = 2;
const int SLOT_ARTIFACT = 3;

class MenuInventory {
private:
	SDL_Surface *screen;
	InputState *inp;
	ItemDatabase *items;
	FontEngine *font;
	StatBlock *stats;
	PowerManager *powers;

	void loadGraphics();
	int areaOver(Point mouse);
	void updateEquipment(int slot);

	SDL_Surface *background;
	WidgetButton *closeButton;
	
public:
	MenuInventory(SDL_Surface *screen, InputState *inp, FontEngine *font, ItemDatabase *items, StatBlock *stats, PowerManager *powers);
	~MenuInventory();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);

	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void drop(Point mouse, ItemStack stack);
	void activate(InputState * input);

	void add( ItemStack stack, int area = CARRIED, int slot = -1);
	void remove(int item);
	void addGold(int count);
	bool buy(ItemStack stack, Point mouse = Point());
	bool sell(ItemStack stack);

	bool full();
	int getItemCountCarried(int item);
	bool isItemEquipped(int item);
	bool requirementsMet(int item);
	
	void applyEquipment(StatBlock *stats, ItemStack *equipped);

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
	
	string log_msg;

};

#endif

