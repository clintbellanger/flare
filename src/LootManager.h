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

#include "ScopedPtr.h"
#include "SmartSurface.h"
#include "ItemManager.h"
#include "Utils.h"
#include "WidgetTooltip.h"

#include <SDL.h>

#include <map>
#include <string>
#include <vector>

class EnemyManager;
class ItemManager;
class MapIso;
class Renderable;
class WidgetTooltip;

struct LootDef {
	ItemStack stack;
	int frame;
	Point pos;
	int gold;
	TooltipData tip;

	void steal(LootDef& ld) {
		stack = ld.stack;
		frame = ld.frame;
		pos = ld.pos;
		gold = ld.gold;
		tip.steal(ld.tip);
	}
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

	ItemManager *items;
	ScopedPtr<WidgetTooltip> tip;
	EnemyManager *enemies;
	MapIso *map;

	// functions
	void loadGraphics();
	void calcTables();
	int lootLevel(int base_level);
	
	SmartSurface flying_loot[64];
	SmartSurface flying_gold[3];
	
	std::string animation_id[64];
	int animation_count;
	
	SmartChunk loot_flip;
	
	Point frame_size;
	int frame_count; // the last frame is the "at-rest" floor loot graphic
	
	// loot refers to ItemManager indices
	std::vector<LootDef> loot; // TODO: change to dynamic list without limits
	
	// Loot table, mapping level and number to item ID.
	std::map<int, std::vector<int> > loot_table;
	
	// animation vars
	int anim_loot_frames;
	int anim_loot_duration;
	
public:
	LootManager(ItemManager *_items, EnemyManager *_enemies, MapIso *_map);

	void handleNewMap();
	void logic();
	void renderTooltips(Point cam);
	void checkEnemiesForLoot();
	void checkMapForLoot();
	bool isFlying(int loot_index);
	void determineLoot(int base_level, Point const& pos);
	int randomItem(int base_level);
	void addLoot(ItemStack stack, Point const& pos);
	void addGold(int count, Point const& pos);
	void removeLoot(int index);
	std::size_t getLootCount() const;
	ItemStack checkPickup(Point const& mouse, Point const& cam, Point const& hero_pos, int &gold, bool inv_full);
	
	Renderable getRender(int index);
	
	int tooltip_margin;
	bool full_msg;
};

#endif
