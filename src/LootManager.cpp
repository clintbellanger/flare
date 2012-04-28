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

#include "LootManager.h"
#include "SharedResources.h"

#include <sstream>

using namespace std;

LootDef::LootDef(const ItemStack &_stack, int _gold, const Point &_pos)
	: stack(_stack)
	, gold(_gold)
	, pos(_pos)
	, frame(0)
	, tip() {
	initTip();
}

void LootDef::initTip() {
	tip.clear();

	if (stack.item) {
		stack.item->initTooltipName(tip, stack.quantity);
	} else {
		tip.addLine(msg->get("%d Gold", gold));
	}
}

void LootDef::animate(ItemManager &items) {
	// animate flying loot
	if (frame < max_frame)
		frame++;

	if (frame == max_frame - 1) {
		if (stack.item)
			items.playSound(*stack.item);
		else
			items.playCoinsSound();
	}
}

void LootDef::renderTooltip(const Point &xcam, const Point &ycam) {
	stringstream ss;
	Point dest(
		VIEW_W_HALF + (pos.x / UNITS_PER_PIXEL_X - xcam.x) - (pos.y / UNITS_PER_PIXEL_X - xcam.y),
		VIEW_H_HALF + (pos.x / UNITS_PER_PIXEL_Y - ycam.x) + (pos.y / UNITS_PER_PIXEL_Y - ycam.y) + (TILE_H / 2)
	);

	// adjust dest.y so that the tooltip floats above the item
	dest.y -= tooltip_margin;

	tip.render(dest, STYLE_TOPLABEL);
}



void LootManager::getRenders(Renderables &dest) {
	for (LootList::const_iterator it = lootList.begin(); it != lootList.end(); ++it) {
		const LootDef &loot = **it;

		Renderable r;
		r.map_pos = loot.pos;
		r.src.x = (loot.frame / LootDef::anim_loot_duration) * 64;
		r.src.y = 0;
		r.src.w = 64;
		r.src.h = 128;
		r.offset = Point(32, 112);
		r.object_layer = true;
		r.tile = Point(0, 0);
		// Right now the animation settings (number of frames, speed, frame size)
		// are hard coded.  At least move these to consts in the header.
		if (loot.stack.item) {
			// item
			for (int i=0; i<animation_count; i++) {
				if (loot.stack.item->loot == animation_id[i])
					r.sprite = flying_loot[i];
			}
		}
		else if (loot.gold > 0) {
			// gold
			if (loot.gold <= 9)
				r.sprite = flying_gold[0];
			else if (loot.gold <= 25)
				r.sprite = flying_gold[1];
			else
				r.sprite = flying_gold[2];
		}

		dest.push_back(r);
	}
}





LootManager::LootManager(ItemManager &_items, EnemyManager &_enemies, MapIso &_map)
	: items(_items)
	, enemies(_enemies)	// we need to be able to read loot state when creatures die
	, map(_map)			// we need to be able to read loot that drops from map containers
{

	loot_count = 0;
	animation_count = 0;

	for (int i=0; i<64; i++) {
		flying_loot[i] = NULL;
		animation_id[i] = "";
	}

	loot_flip = NULL;
#if 0
	// reset current map loot
	for (int i=0; i<256; i++) {
		LootDef &sloot = loot[i];

		sloot.pos = Point(0, 0);
		sloot.frame = 0;
		sloot.stack.item = NULL;
		sloot.stack.quantity = 0;
		sloot.gold = 0;
	}
#endif

	// reset loot table
	for (int lvl=0; lvl<15; lvl++) {
		loot_table_count[lvl] = 0;
		for (int num=0; num<256; num++) {
			loot_table[lvl][num] = 0;
		}
	}

	loadGraphics();
	calcTables();
    if (audio == true)
        loot_flip = Mix_LoadWAV(mods->locate("soundfx/flying_loot.ogg").c_str());
	full_msg = false;


}

/**
 * The "loot" variable on each item refers to the "flying loot" animation for that item.
 * Here we load all the animations used by the item database.
 */
void LootManager::loadGraphics() {

	string anim_id;
	bool new_anim;

	// check all items in the item database
	for (int i=0; i<MAX_ITEM_ID; i++) {
		anim_id = items.getItem(i).loot;

		new_anim = true;

		if (anim_id != "") {

			// do we have this animation loaded already?
			for (int j=0; j<animation_count; j++) {
				if (anim_id == animation_id[j]) new_anim = false;
			}

			if (new_anim) {
				flying_loot[animation_count] = IMG_Load(mods->locate("images/loot/" + anim_id + ".png").c_str());

				if (flying_loot[animation_count]) {
					animation_id[animation_count] = anim_id;
					animation_count++;
				}
			}
		}
	}

	// gold
	flying_gold[0] = IMG_Load(mods->locate("images/loot/coins5.png").c_str());
	flying_gold[1] = IMG_Load(mods->locate("images/loot/coins25.png").c_str());
	flying_gold[2] = IMG_Load(mods->locate("images/loot/coins100.png").c_str());

	// set magic pink transparency
	for (int i=0; i<animation_count; i++) {
		SDL_SetColorKey( flying_loot[i], SDL_SRCCOLORKEY, SDL_MapRGB(flying_loot[i]->format, 255, 0, 255) );

		// optimize
		SDL_Surface *cleanup = flying_loot[i];
		flying_loot[i] = SDL_DisplayFormatAlpha(flying_loot[i]);
		SDL_FreeSurface(cleanup);
	}
	for (int i=0; i<3; i++) {
		SDL_SetColorKey( flying_gold[i], SDL_SRCCOLORKEY, SDL_MapRGB(flying_gold[i]->format, 255, 0, 255) );

		// optimize
		SDL_Surface *cleanup = flying_gold[i];
		flying_gold[i] = SDL_DisplayFormatAlpha(flying_gold[i]);
		SDL_FreeSurface(cleanup);
	}
}

/**
 * Each item has a level, roughly associated with what level monsters drop that item.
 * Each item also has a quality which affects how often it drops.
 * Here we calculate loot probability by level so that when loot drops we
 * can quickly choose what loot should drop.
 */
void LootManager::calcTables() {

	int level;

	for (int i=0; i<1024; i++) {
		const Item &item = items.getItem(i);
		level = item.level;
		if (level > 0) {
			if (item.quality == ITEM_QUALITY_LOW) {
				for (int j=0; j<RARITY_LOW; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (item.quality == ITEM_QUALITY_NORMAL) {
				for (int j=0; j<RARITY_NORMAL; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (item.quality == ITEM_QUALITY_HIGH) {
				for (int j=0; j<RARITY_HIGH; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (item.quality == ITEM_QUALITY_EPIC) {
				for (int j=0; j<RARITY_EPIC; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
		}
	}
}

void LootManager::clear() {
	while (!lootList.empty()) {
		delete lootList.front();
		lootList.pop_front();
	}
}

void LootManager::logic() {
	for (LootList::iterator i = lootList.begin(); i != lootList.end(); ++i) {
		(*i)->animate(items);
	}
	checkEnemiesForLoot();
	checkMapForLoot();
}

/**
 * Show all tooltips for loot on the floor
 */
void LootManager::renderTooltips(const Point &cam) {

	Point xcam (cam / UNITS_PER_PIXEL_X);
	Point ycam (cam / UNITS_PER_PIXEL_Y);

	for (LootList::iterator i = lootList.begin(); i != lootList.end(); ++i) {
		if (!(*i)->isFlying()) {
			(*i)->renderTooltip(xcam, ycam);
		}

	}


}

/**
 * Enemies that drop loot raise a "loot_drop" flag to notify this loot
 * manager to create loot based on that creature's level and position.
 */
void LootManager::checkEnemiesForLoot() {
	for (int i=0; i<enemies.enemy_count; i++) {
		if (enemies.enemies[i]->loot_drop) {
			int loot_id = enemies.enemies[i]->stats.quest_loot_id;

			if (loot_id) {
				// quest loot
				addLoot(ItemStack(&items.getItem(loot_id), 1), enemies.enemies[i]->stats.pos);
			}
			else {
				// random loot
				determineLoot(enemies.enemies[i]->stats.level, enemies.enemies[i]->stats.pos);
			}

			enemies.enemies[i]->loot_drop = false;
		}
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

	while (!map.loot.empty()) {
		ec = &map.loot.front();
		p.x = ec->x;
		p.y = ec->y;

		if (ec->s == "random") {
			determineLoot(ec->z, p);
		}
		else if (ec->s == "id") {
			new_loot.item = &items.getItem(ec->z);
			new_loot.quantity = 1;
			addLoot(new_loot, p);
		}
		else if (ec->s == "currency") {
			addGold(ec->z, p);
		}
		map.loot.pop();
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
	if (x <= 4) actual = base_level-3;
	else if (x <= 14) actual = base_level-2;
	else if (x <= 34) actual = base_level-1;
	else if (x <= 64) actual = base_level;
	else if (x <= 84) actual = base_level+1;
	else if (x <= 94) actual = base_level+2;
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
void LootManager::determineLoot(int base_level, Point pos) {
	int level = lootLevel(base_level);

	if (level > 0 && loot_table_count[level] > 0) {

		// coin flip whether the treasure is cash or items
		if (rand() % 2 == 0) {
			int roll = rand() % loot_table_count[level];
			const Item &item = items.getItem(loot_table[level][roll]);
			ItemStack new_loot(&item, rand() % item.rand_loot + 1);
			addLoot(new_loot, pos);
		}
		else {
			// gold range is level to 3x level
			addGold(rand() % (level * 2) + level, pos);
		}
	}
}

/**
 * Choose a random item.
 * Useful for filling in a Vendor's wares.
 */
const Item *LootManager::randomItem(int base_level) {
	int level = lootLevel(base_level);
	if (level > 0 && loot_table_count[level] > 0) {
		int roll = rand() % loot_table_count[level];
		return &items.getItem(loot_table[level][roll]);
	}
	return NULL;
}

void LootManager::addLoot(const ItemStack &stack, const Point &pos) {
	// TODO: z-sort insert?
	lootList.push_back(new LootDef(stack, 0, pos));
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

void LootManager::addGold(int gold, const Point &pos) {
	lootList.push_back(new LootDef(ItemStack(), gold, pos));
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

/**
 * Click on the map to pick up loot.  We need the camera position to translate
 * screen coordinates to map locations.  We need the hero position because
 * the hero has to be within range to pick up an item.
 */
ItemStack LootManager::checkPickup(const Point &mouse, const Point &cam, const Point &hero_pos, int &gold, bool inv_full) {
	Point p;
	SDL_Rect r;
	ItemStack loot_stack;
	gold = 0;

	// I'm starting at the end of the loot list so that more recently-dropped
	// loot is picked up first.  If a player drops several loot in the same
	// location, picking it back up will work like a stack.
	for (LootList::reverse_iterator i = lootList.rbegin(), end = lootList.rend(); i != end; ++i) {
		LootDef &loot = **i;

		// loot close enough to pickup?
		if (loot.pos.dist(hero_pos) < LOOT_RANGE && !loot.isFlying()) {

			p = map_to_screen(loot.pos, cam);

			r.w = 32;
			r.h = 48;
			r.x = p.x - 16;
			r.y = p.y - 32;

			// clicked in pickup hotspot?
			if (mouse.x > r.x && mouse.x < r.x + r.w &&
				mouse.y > r.y && mouse.y < r.y + r.h) {

				if (loot.stack.item) {
					if (!inv_full) {
						loot_stack = loot.stack;
					}
					else {
						full_msg = true;
						return ItemStack();
					}
				}
				else if (loot.gold > 0) {
					gold = loot.gold;
				}
				else {
					continue;
				}
				lootList.erase(--(i.base())); // sorry this looks funky
				return loot_stack;
			}
		}
	}
	return ItemStack();
}

LootManager::~LootManager() {

	for (int i=0; i<64; i++)
		if (flying_loot[i])
			SDL_FreeSurface(flying_loot[i]);
	for (int i=0; i<3; i++)
		if (flying_gold[i])
			SDL_FreeSurface(flying_gold[i]);
	if (loot_flip) Mix_FreeChunk(loot_flip);

	// clear loot tooltips to free buffer memory
	clear();
}
