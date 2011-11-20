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
 * class LootManager
 *
 * Handles floor loot
 */
 
#ifndef LOOT_MANAGER_H
#define LOOT_MANAGER_H

#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "Utils.h"
#include "ItemDatabase.h"
#include "MenuTooltip.h"
#include "EnemyManager.h"
#include "MessageEngine.h"

struct LootDef {
	ItemStack stack;
	int frame;
	Point pos;
	int gold;
};


// this means that normal items are 10x more common than epic items
// these numbers have to be balanced by various factors
const int RARITY_LOW = 7;
const int RARITY_NORMAL = 10;
const int RARITY_HIGH = 3;
const int RARITY_EPIC = 1;

// how close (map units) does the hero have to be to pick up loot?
const int LOOT_RANGE = 3 * UNITS_PER_TILE;

class LootManager {
private:

	ItemDatabase *items;
	MenuTooltip *tip;
	EnemyManager *enemies;
	MapIso *map;

	// functions
	void loadGraphics();
	void calcTables();
	int lootLevel(int base_level);
	
	SDL_Surface *flying_loot[64];
	SDL_Surface *flying_gold[3];
	
	string animation_id[64];
	int animation_count;
	
	Mix_Chunk *loot_flip;
	
	Point frame_size;
	int frame_count; // the last frame is the "at-rest" floor loot graphic
	
	// loot refers to ItemDatabase indices
	LootDef loot[256]; // TODO: change to dynamic list without limits
	
	// loot tables multiplied out
	// currently loot can range from levels 0-20
	int loot_table[21][1024]; // level, number.  the int is an item id
	int loot_table_count[21]; // total number per level
	
	// animation vars
	int anim_loot_frames;
	int anim_loot_duration;
	
public:
	LootManager(ItemDatabase *_items, MenuTooltip *_tip, EnemyManager *_enemies, MapIso *_map);
	~LootManager();

	void handleNewMap();
	void logic();
	void renderTooltips(Point cam);
	void checkEnemiesForLoot();
	void checkMapForLoot();
	bool isFlying(int loot_index);
	void determineLoot(int base_level, Point pos);
	int randomItem(int base_level);
	void addLoot(ItemStack stack, Point pos);
	void addGold(int count, Point pos);
	void removeLoot(int index);
	ItemStack checkPickup(Point mouse, Point cam, Point hero_pos, int &gold, bool inv_full);
	
	Renderable getRender(int index);
	
	int tooltip_margin;
	int loot_count;
	bool full_msg;
	
};

#endif
