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
 
#include "LootManager.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "MapIso.h"
#include "MessageEngine.h"
#include "ModManager.h"
#include "SharedResources.h"

#include <SDL_image.h>
 
#include <sstream>

using namespace std;


LootManager::LootManager(ItemManager *_items, EnemyManager *_enemies, MapIso *_map) {
	items = _items;
	enemies = _enemies; // we need to be able to read loot state when creatures die
	map = _map; // we need to be able to read loot that drops from map containers

	tip.reset(new WidgetTooltip());

	// HACK: This makes sure that the vector is fairly big, so that we never
	// reallocate and move LootDefs.  Until we come up with reasonable move
	// semantics for TooltipData, it's unclear what else we can do to mitigate
	// this (apart from making it constant-size, but that would hardly be much
	// better).
	loot.reserve(256);
	
	tooltip_margin = 32; // pixels between loot drop center and label
	
	animation_count = 0;
	
	loadGraphics();
	calcTables();
	loot_flip.reset_and_load("soundfx/flying_loot.ogg");
	full_msg = false;
	
	anim_loot_frames = 6;
	anim_loot_duration = 3;
	
}

/**
 * The "loot" variable on each item refers to the "flying loot" animation for that item.
 * Here we load all the animations used by the item database.
 */
void LootManager::loadGraphics() {

	string anim_id;

	// check all items in the item database
	std::map<int, Item>::iterator end = items->items.end();
	for (std::map<int, Item>::iterator it = items->items.begin(); it != end; ++it) {
		anim_id = it->second.loot;
		
		bool new_anim = true;
		
		if (!anim_id.empty()) {
			
			// do we have this animation loaded already?
			for (int j=0; j<animation_count; j++) {
				if (anim_id == animation_id[j]) new_anim = false;
			}
			
			if (new_anim) {
				flying_loot[animation_count].reset_and_load("images/loot/" + anim_id + ".png");
				
				if (flying_loot[animation_count]) {
					animation_id[animation_count] = anim_id;
					animation_count++;
				}
			}
		}
	}
	
	// gold
	flying_gold[0].reset_and_load("images/loot/coins5.png");
	flying_gold[1].reset_and_load("images/loot/coins25.png");
	flying_gold[2].reset_and_load("images/loot/coins100.png");
	
	// set magic pink transparency
	for (int i=0; i<animation_count; i++) {
		flying_loot[i].set_color_key(SDL_SRCCOLORKEY, flying_loot[i].map_rgb(255, 0, 255)); 
		flying_loot[i].display_format_alpha();
	}
	for (int i=0; i<3; i++) {
		flying_gold[i].set_color_key(SDL_SRCCOLORKEY, flying_gold[i].map_rgb(255, 0, 255)); 
		flying_gold[i].display_format_alpha();
	}
}

static int convertQualityToRarity(int quality) {
	switch (quality) {
		case ITEM_QUALITY_LOW:
			return RARITY_LOW;
		case ITEM_QUALITY_NORMAL:
			return RARITY_NORMAL;
		case ITEM_QUALITY_HIGH:
			return RARITY_HIGH;
		case ITEM_QUALITY_EPIC:
			return RARITY_EPIC;
	}
	return 0;
}

/**
 * Each item has a level, roughly associated with what level monsters drop that item.
 * Each item also has a quality which affects how often it drops.
 * Here we calculate loot probability by level so that when loot drops we
 * can quickly choose what loot should drop.
 */
void LootManager::calcTables() {
	for (std::size_t i=0; i<items->items.size(); i++) {
		int level = items->items[i].level;
		if (level <= 0)
			continue;
		int repeats = convertQualityToRarity(items->items[i].quality);
		for (int j=0; j<repeats; j++)
			loot_table[level].push_back(i);
	}
}

void LootManager::handleNewMap() {
	loot.clear();
}

void LootManager::logic() {
	int max_frame = anim_loot_frames * anim_loot_duration - 1;
	
	std::vector<LootDef>::iterator end = loot.end();
	for (std::vector<LootDef>::iterator it = loot.begin(); it != end; ++it) {
		// animate flying loot
		if (it->frame < max_frame)
			it->frame++;

		if (it->frame == max_frame-1) {
			if (it->stack.item > 0)
				items->playSound(it->stack.item);
			else
				items->playCoinsSound();
		}
	}
	
	checkEnemiesForLoot();
	checkMapForLoot();
}

/**
 * If an item is flying, it hasn't completed its "flying loot" animation.
 * Only allow loot to be picked up if it is grounded.
 */
bool LootManager::isFlying(int loot_index) {
	int max_frame = anim_loot_frames * anim_loot_duration - 1;
	if (loot[loot_index].frame == max_frame) return false;
	return true;
}

/**
 * Show all tooltips for loot on the floor
 */
void LootManager::renderTooltips(Point cam) {

	Point xcam;
	Point ycam;
	xcam.x = cam.x/UNITS_PER_PIXEL_X;
	xcam.y = cam.y/UNITS_PER_PIXEL_X;
	ycam.x = cam.x/UNITS_PER_PIXEL_Y;
	ycam.y = cam.y/UNITS_PER_PIXEL_Y;
	
	Point dest;
	stringstream ss;
	
	int max_frame = anim_loot_frames * anim_loot_duration - 1;
	
	std::vector<LootDef>::iterator end = loot.end();
	for (std::vector<LootDef>::iterator it = loot.begin(); it != end; ++it) {
		if (it->frame != max_frame)
			continue;
		dest.x = VIEW_W_HALF + (it->pos.x/UNITS_PER_PIXEL_X - xcam.x) - (it->pos.y/UNITS_PER_PIXEL_X - xcam.y);
		dest.y = VIEW_H_HALF + (it->pos.x/UNITS_PER_PIXEL_Y - ycam.x) + (it->pos.y/UNITS_PER_PIXEL_Y - ycam.y) + (TILE_H/2);
	
		// adjust dest.y so that the tooltip floats above the item
		dest.y -= tooltip_margin;

		// create tooltip data if needed
		if (!it->tip.tip_buffer) {

			if (it->stack.item > 0) {
				it->tip = items->getShortTooltip(it->stack);
			}
			else {
				it->tip.num_lines = 1;
				it->tip.colors[0] = FONT_WHITE;
				ss << msg->get("%d Gold", it->gold);
				it->tip.lines[0] = ss.str();
				ss.str("");
			}
		}
		
		tip->render(it->tip, dest, STYLE_TOPLABEL);
	}
}

/**
 * Enemies that drop loot raise a "loot_drop" flag to notify this loot
 * manager to create loot based on that creature's level and position.
 */
void LootManager::checkEnemiesForLoot() {
	ItemStack istack;
	istack.quantity = 1;
	
	PtrVector<Enemy>::iterator end = enemies->enemies.end();
	for(PtrVector<Enemy>::iterator it = enemies->enemies.begin(); it != end; ++it) {
		if (!it->loot_drop)
			continue;
		
		if (it->stats.quest_loot_id != 0) {				
			// quest loot
			istack.item = it->stats.quest_loot_id;
			addLoot(istack, it->stats.pos);
		}
		else {
			// random loot
			determineLoot(it->stats.level, it->stats.pos);
		}
		
		it->loot_drop = false;
	}
}

/**
 * As map events occur, some might have a component named "loot"
 * Loot is created at component x,y
 */
void LootManager::checkMapForLoot() {
	Point p;
	Event_Component *ec;
	ItemStack new_loot;
	
	while (!map->loot.empty()) {
		ec = &map->loot.front();
		p.x = ec->x;
		p.y = ec->y;
		
		if (ec->s == "random") {
			determineLoot(ec->z, p);
		}
		else if (ec->s == "id") {
			new_loot.item = ec->z;
			new_loot.quantity = 1;
			addLoot(new_loot, p);
		}
		else if (ec->s == "currency") {
			addGold(ec->z, p);
		}
		map->loot.pop();
	}
}

/**
 * Monsters don't just drop loot equal to their level
 * The loot level spread is a bell curve
 */
int LootManager::lootLevel(int base_level) {

	int x = rand() % 100;
	int actual;
	
	// this loot bell curve is +/- 3 levels
	// percents: 5,10,20,30,20,10,5
	if (x < 5) actual = base_level-3;
	else if (x < 15) actual = base_level-2;
	else if (x < 35) actual = base_level-1;
	else if (x < 65) actual = base_level;
	else if (x < 85) actual = base_level+1;
	else if (x < 95) actual = base_level+2;
	else actual = base_level+3;
	
	if (actual < 1) actual = 0;
	if (actual > 20) actual = base_level;
	
	return actual;
}

/**
 * This function is called when there definitely is a piece of loot dropping
 * base_level represents the average quality of this loot
 * calls addLoot()
 */
void LootManager::determineLoot(int base_level, Point const& pos) {
	int level = lootLevel(base_level);
	ItemStack new_loot;

	if (level == 0 || loot_table[level].size() == 0)
		return;
	
	// coin flip whether the treasure is cash or items
	if (rand() % 2 == 0) {
		int roll = rand() % loot_table[level].size();
		new_loot.item = loot_table[level][roll];
		new_loot.quantity = rand() % items->items[new_loot.item].rand_loot + 1;
		addLoot(new_loot, pos);
	}
	else {
		// gold range is level to 3x level
		addGold(rand() % (level * 2) + level, pos);
	}
}

/**
 * Choose a random item.
 * Useful for filling in a Vendor's wares.
 */
int LootManager::randomItem(int base_level) {
	int level = lootLevel(base_level);
	if (level == 0 || loot_table[level].size() == 0)
		return 0;
	int roll = rand() % loot_table[level].size();
	return loot_table[level][roll];
}

void LootManager::addLoot(ItemStack stack, Point const& pos) {
	// TODO: z-sort insert?
	LootDef ld;
	ld.stack = stack;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;
	ld.frame = 0;
	ld.gold = 0;
	FlareAssert(!ld.tip.tip_buffer && "Buffer unexpectedly non-empty.");
	FlareSoftAssert(0, "Loot entry");
	loot.push_back(ld);
	FlareSoftAssert(0, "Loot exit");
	loot_flip.play_channel(-1, 0);
}

void LootManager::addGold(int count, Point const& pos) {
	LootDef ld;
	ld.stack.item = 0;
	ld.stack.quantity = 0;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;
	ld.frame = 0;
	ld.gold = count;
	FlareAssert(!ld.tip.tip_buffer && "Buffer unexpectedly non-empty.");
	FlareSoftAssert(0, "Gold entry");
	loot.push_back(ld);
	FlareSoftAssert(0, "Gold exit");
	loot_flip.play_channel(-1, 0);
}


/**
 * Remove one loot from the array, preserving sort order
 */
void LootManager::removeLoot(int index) {
	FlareAssert(index >= 0);
	FlareAssert(static_cast<std::size_t>(index) < loot.size());
	loot[index].steal(loot.back());
	loot.pop_back();
}

std::size_t LootManager::getLootCount() const {
	return loot.size();
}

/**
 * Click on the map to pick up loot.  We need the camera position to translate
 * screen coordinates to map locations.  We need the hero position because
 * the hero has to be within range to pick up an item.
 */
ItemStack LootManager::checkPickup(Point const& mouse, Point const& cam, Point const& hero_pos, int &gold, bool inv_full) {
	Point p;
	SDL_Rect r;
	ItemStack loot_stack;
	gold = 0;	
	loot_stack.item = 0;
	loot_stack.quantity = 0;
	
	// I'm starting at the end of the loot list so that more recently-dropped
	// loot is picked up first.  If a player drops several loot in the same
	// location, picking it back up will work like a stack.
	for (int i=loot.size()-1; i>=0; i--) {

		// loot close enough to pickup?
		if (abs(hero_pos.x - loot[i].pos.x) < LOOT_RANGE && abs(hero_pos.y - loot[i].pos.y) < LOOT_RANGE && !isFlying(i)) {

			p = map_to_screen(loot[i].pos.x, loot[i].pos.y, cam.x, cam.y);
				
			r.w = 32;
			r.h = 48;
			r.x = p.x - 16;
			r.y = p.y - 32;
		
			// clicked in pickup hotspot?
			if (mouse.x > r.x && mouse.x < r.x+r.w &&
				mouse.y > r.y && mouse.y < r.y+r.h) {
				
				if (loot[i].stack.item > 0 && !inv_full) {
					loot_stack = loot[i].stack;
					removeLoot(i);
					return loot_stack;
				}
				else if (loot[i].stack.item > 0) {
					full_msg = true;
				}
				else if (loot[i].gold > 0) {
					gold = loot[i].gold;
					removeLoot(i);

					return loot_stack;
				}
			}
		}
	}
	return loot_stack;
}

Renderable LootManager::getRender(int index) {
	
	Renderable r;
	r.map_pos.x = loot[index].pos.x;
	r.map_pos.y = loot[index].pos.y;
	
	// Right now the animation settings (number of frames, speed, frame size)
	// are hard coded.  At least move these to consts in the header.

	r.src.x = (loot[index].frame / anim_loot_duration) * 64;
	r.src.y = 0;
	r.src.w = 64;
	r.src.h = 128;
	r.offset.x = 32;
	r.offset.y = 112;
	r.object_layer = true;

	if (loot[index].stack.item > 0) {
		// item
		for (int i=0; i<animation_count; i++) {
			if (items->items[loot[index].stack.item].loot == animation_id[i])
				r.sprite = flying_loot[i].get();
		}
	}
	else if (loot[index].gold > 0) {
		// gold
		if (loot[index].gold <= 9)
			r.sprite = flying_gold[0].get();
		else if (loot[index].gold <= 25)
			r.sprite = flying_gold[1].get();
		else
			r.sprite = flying_gold[2].get();
	}

	return r;	
}

