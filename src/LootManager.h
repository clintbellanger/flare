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
 * class LootManager
 *
 * Handles floor loot
 */

#ifndef LOOT_MANAGER_H
#define LOOT_MANAGER_H

#include "Utils.h"
#include "ItemManager.h"
#include "WidgetTooltip.h"
#include "EnemyManager.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <list>

using std::string;


class LootDef : Uncopyable {
	friend class LootManager;
public:
	// animation vars
	const static int anim_loot_frames = 6;
	const static int anim_loot_duration = 3;
	const static int max_frame = anim_loot_frames * anim_loot_duration - 1;
	const static int tooltip_margin = 32; 	/**< pixels between loot drop center and label */

private:
	ItemStack stack;
	int gold;
	Point pos;
	int frame;
	WidgetTooltip tip;

public:
	LootDef(const ItemStack &stack, int gold, const Point &pos);

	/**
	* If an item is flying, it hasn't completed its "flying loot" animation.
	* Only allow loot to be picked up if it is grounded.
	*/
	bool isFlying() const				{return frame < max_frame;}
	void animate(ItemManager &items);
	void renderTooltip(const Point &xcam, const Point &ycam);

private:
	void initTip();
};

// this means that normal items are 10x more common than epic items
// these numbers have to be balanced by various factors
const int RARITY_LOW = 7;
const int RARITY_NORMAL = 10;
const int RARITY_HIGH = 3;
const int RARITY_EPIC = 1;

// how close (map units) does the hero have to be to pick up loot?
const int LOOT_RANGE = 3 * UNITS_PER_TILE;

class LootManager : private Uncopyable {
public:
	typedef std::list<LootDef *> LootList;
	//typedef const std::list<const LootDef *> ConstLootList;

private:

	ItemManager &items;
	EnemyManager &enemies;
	MapIso &map;

	// functions
	void loadGraphics();
	void calcTables();
	int lootLevel(int base_level);

	SDL_Surface *flying_loot[64];
	SDL_Surface *flying_gold[3];

	std::string animation_id[64];
	int animation_count;

	Mix_Chunk *loot_flip;

	Point frame_size;
	int frame_count; // the last frame is the "at-rest" floor loot graphic

	// loot refers to ItemManager indices
	LootList lootList;

	// loot tables multiplied out
	// currently loot can range from levels 0-20
	int loot_table[21][1024]; // level, number.  the int is an item id
	int loot_table_count[21]; // total number per level



public:
	LootManager(ItemManager &_items, EnemyManager &_enemies, MapIso &_map);
	~LootManager();

	void handleNewMap()				{clear();}
	void logic();
	void renderTooltips(const Point &cam);
	void checkEnemiesForLoot();
	void checkMapForLoot();
	void determineLoot(int base_level, Point pos);
	const Item *randomItem(int base_level);
	void addLoot(const ItemStack &stack, const Point &pos);
	void addGold(int count, const Point &pos);
	ItemStack checkPickup(const Point &mouse, const Point &cam, const Point &hero_pos, int &gold, bool inv_full);

	void getRenders(Renderables &dest);

	int tooltip_margin;
	int loot_count;
	bool full_msg;

	/** Return a const version of lootList (reinterpret_cast used to change underlying const-ness) */
	//ConstLootList &getLootList() {return reinterpret_cast<ConstLootList &>(lootList);}

private:
	void clear();

};

#endif
