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
 * class ItemManager
 */

#include "ItemManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "UtilsFileSystem.h"
#include "WidgetLabel.h"

#include <sstream>
#include <fstream>

using namespace std;

const float Item::vendor_ratio = 4.f; // this means scrap/vendor pays 1/4th price to buy items from hero
const float Item::vendor_gem_ratio = 2.f; // but pays 1/2 price for gems


ItemManager::ItemManager() {

	items = new Item[MAX_ITEM_ID];

	for (int i=0; i<MAX_ITEM_ID; i++) {
		items[i].bonus_stat = new string[ITEM_MAX_BONUSES];
		items[i].bonus_val = new int[ITEM_MAX_BONUSES];
		for (int j=0; j<ITEM_MAX_BONUSES; j++) {
			items[i].bonus_stat[j] = "";
			items[i].bonus_val[j] = 0;
		}
	}

	loadAll();
	loadSounds();
	loadIcons();
}

/**
 * Load all items files in all mods
 */
void ItemManager::loadAll() {

	string test_path;

	// load each items.txt file. Individual item IDs can be overwritten with mods.
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/items/items.txt";

		if (fileExists(test_path)) {
			this->load(test_path);
		}
	}

}

/**
 * Load a specific items file
 *
 * @param filename The full path and name of the file to load
 */
void ItemManager::load(const string& filename) {
	FileParser infile;
	int id = 0;
	string s;
	int bonus_counter = 0;

	if (infile.open(filename)) {

		while (infile.next()) {
			if (infile.key == "id") {
				id = atoi(infile.val.c_str());
				items[id].id = id;

				// new item, reset bonus counter
				bonus_counter = 0;
			}
			else if (infile.key == "name")
				items[id].name = msg->get(infile.val);
			else if (infile.key == "level")
				items[id].level = atoi(infile.val.c_str());
			else if (infile.key == "icon") {
				items[id].icon32 = atoi(infile.nextValue().c_str());
				items[id].icon64 = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "quality") {
				if (infile.val == "low")
					items[id].quality = ITEM_QUALITY_LOW;
				else if (infile.val == "high")
					items[id].quality = ITEM_QUALITY_HIGH;
				else if (infile.val == "epic")
					items[id].quality = ITEM_QUALITY_EPIC;
			}
			else if (infile.key == "type") {
				if (infile.val == "main")
					items[id].type = ITEM_TYPE_MAIN;
				else if (infile.val == "body")
					items[id].type = ITEM_TYPE_BODY;
				else if (infile.val == "off")
					items[id].type = ITEM_TYPE_OFF;
				else if (infile.val == "artifact")
					items[id].type = ITEM_TYPE_ARTIFACT;
				else if (infile.val == "consumable")
					items[id].type = ITEM_TYPE_CONSUMABLE;
				else if (infile.val == "gem")
					items[id].type = ITEM_TYPE_GEM;
				else if (infile.val == "quest")
					items[id].type = ITEM_TYPE_QUEST;
			}
			else if (infile.key == "dmg") {
				items[id].dmg_min = atoi(infile.nextValue().c_str());
				if (infile.val.length() > 0)
					items[id].dmg_max = atoi(infile.nextValue().c_str());
				else
					items[id].dmg_max = items[id].dmg_min;
			}
			else if (infile.key == "abs") {
				items[id].abs_min = atoi(infile.nextValue().c_str());
				if (infile.val.length() > 0)
					items[id].abs_max = atoi(infile.nextValue().c_str());
				else
					items[id].abs_max = items[id].abs_min;
			}
			else if (infile.key == "req") {
				s = infile.nextValue();
				if (s == "p")
					items[id].req_stat = REQUIRES_PHYS;
				else if (s == "m")
					items[id].req_stat = REQUIRES_MENT;
				else if (s == "o")
					items[id].req_stat = REQUIRES_OFF;
				else if (s == "d")
					items[id].req_stat = REQUIRES_DEF;
				items[id].req_val = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "bonus") {
				if (bonus_counter < ITEM_MAX_BONUSES) {
					items[id].bonus_stat[bonus_counter] = infile.nextValue();
					items[id].bonus_val[bonus_counter] = atoi(infile.nextValue().c_str());
					bonus_counter++;
				}
			}
			else if (infile.key == "sfx") {
				if (infile.val == "book")
					items[id].sfx = SFX_BOOK;
				else if (infile.val == "cloth")
					items[id].sfx = SFX_CLOTH;
				else if (infile.val == "coins")
					items[id].sfx = SFX_COINS;
				else if (infile.val == "gem")
					items[id].sfx = SFX_GEM;
				else if (infile.val == "leather")
					items[id].sfx = SFX_LEATHER;
				else if (infile.val == "metal")
					items[id].sfx = SFX_METAL;
				else if (infile.val == "page")
					items[id].sfx = SFX_PAGE;
				else if (infile.val == "maille")
					items[id].sfx = SFX_MAILLE;
				else if (infile.val == "object")
					items[id].sfx = SFX_OBJECT;
				else if (infile.val == "heavy")
					items[id].sfx = SFX_HEAVY;
				else if (infile.val == "wood")
					items[id].sfx = SFX_WOOD;
				else if (infile.val == "potion")
					items[id].sfx = SFX_POTION;
			}
			else if (infile.key == "gfx")
				items[id].gfx = infile.val;
			else if (infile.key == "loot")
				items[id].loot = infile.val;
			else if (infile.key == "power")
				items[id].power = atoi(infile.val.c_str());
			else if (infile.key == "power_mod")
				items[id].power_mod = atoi(infile.val.c_str());
			else if (infile.key == "power_desc")
				items[id].power_desc = msg->get(infile.val);
			else if (infile.key == "price")
				items[id].price = atoi(infile.val.c_str());
			else if (infile.key == "max_quantity")
				items[id].max_quantity = atoi(infile.val.c_str());
			else if (infile.key == "rand_loot")
				items[id].rand_loot = atoi(infile.val.c_str());
			else if (infile.key == "rand_vendor")
				items[id].rand_vendor = atoi(infile.val.c_str());
			else if (infile.key == "pickup_status")
				items[id].pickup_status = infile.val;
			else if (infile.key == "stepfx")
				items[id].stepfx = infile.val;

		}
		infile.close();
	}
}

void ItemManager::loadSounds() {

    if (audio == true) {
        sfx[SFX_BOOK] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_book.ogg").c_str());
        sfx[SFX_CLOTH] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_cloth.ogg").c_str());
        sfx[SFX_COINS] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_coins.ogg").c_str());
        sfx[SFX_GEM] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_gem.ogg").c_str());
        sfx[SFX_LEATHER] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_leather.ogg").c_str());
        sfx[SFX_METAL] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_metal.ogg").c_str());
        sfx[SFX_PAGE] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_page.ogg").c_str());
        sfx[SFX_MAILLE] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_maille.ogg").c_str());
        sfx[SFX_OBJECT] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_object.ogg").c_str());
        sfx[SFX_HEAVY] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_heavy.ogg").c_str());
        sfx[SFX_WOOD] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_wood.ogg").c_str());
        sfx[SFX_POTION] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_potion.ogg").c_str());
    } else {
        sfx[SFX_BOOK] = NULL;
        sfx[SFX_CLOTH] = NULL;
        sfx[SFX_COINS] = NULL;
        sfx[SFX_GEM] = NULL;
        sfx[SFX_LEATHER] = NULL;
        sfx[SFX_METAL] = NULL;
        sfx[SFX_PAGE] = NULL;
        sfx[SFX_MAILLE] = NULL;
        sfx[SFX_OBJECT] = NULL;
        sfx[SFX_HEAVY] = NULL;
        sfx[SFX_WOOD] = NULL;
    }
}

/**
 * Icon sets
 */
void ItemManager::loadIcons() {

	icons32 = IMG_Load(mods->locate("images/icons/icons32.png").c_str());
	icons64 = IMG_Load(mods->locate("images/icons/icons64.png").c_str());

	if(!icons32 || !icons64) {
		fprintf(stderr, "Couldn't load icons: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = icons32;
	icons32 = SDL_DisplayFormatAlpha(icons32);
	SDL_FreeSurface(cleanup);

	cleanup = icons64;
	icons64 = SDL_DisplayFormatAlpha(icons64);
	SDL_FreeSurface(cleanup);
}

/**
 * Renders icons at 32px size or 64px size
 * Also display the stack size
 */
void ItemManager::renderIcon(ItemStack stack, int x, int y, int size) {
	int columns;

	if (!stack.item)
		return;

	dest.x = x;
	dest.y = y;
	src.w = src.h = dest.w = dest.h = size;
	if (size == ICON_SIZE_32) {
		columns = icons32->w / 32;
		src.x = (stack.item->icon32 % columns) * size;
		src.y = (stack.item->icon32 / columns) * size;
		SDL_BlitSurface(icons32, &src, screen, &dest);
	}
	else if (size == ICON_SIZE_64) {
		columns = icons64->w / 64;
		src.x = (stack.item->icon64 % columns) * size;
		src.y = (stack.item->icon64 / columns) * size;
		SDL_BlitSurface(icons64, &src, screen, &dest);
	}

	if( stack.quantity > 1 || stack.item->max_quantity > 1) {
		// stackable item : show the quantity
		stringstream ss;
		ss << stack.quantity;

		WidgetLabel label;
		label.set(dest.x + 2, dest.y + 2, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), FONT_WHITE);
		label.render();
	}
}

void ItemManager::playSound(const Item &item) {
    if (item.sfx != SFX_NONE)
        if (sfx[item.sfx])
            Mix_PlayChannel(-1, sfx[item.sfx], 0);
}

void ItemManager::playCoinsSound() {
    if (sfx[SFX_COINS])
        Mix_PlayChannel(-1, sfx[SFX_COINS], 0);
}

void Item::initTooltipName(WidgetTooltip &tip, unsigned quantity) const {
	int color;
	stringstream ss;

	// name
	if (quantity > 1) {
		ss << quantity << " " << name;
	} else {
		ss << name;
	}

	// color quality
	switch (quality) {
		case ITEM_QUALITY_LOW:	color = FONT_GRAY; break;
		case ITEM_QUALITY_HIGH:	color = FONT_GREEN; break;
		case ITEM_QUALITY_EPIC:	color = FONT_BLUE; break;
	}

	tip.addLine(ss.str(), color);
}


static string getRangeMsg(const char *str, int minval, int maxval) {
	string fmt(str);
	if(minval == maxval) {
		return msg->get(fmt, minval);
	} else {
		fmt += "-%d";
		return msg->get(fmt, minval, maxval);
	}
}

/**
 * Create detailed tooltip showing all relevant item info
 */
void Item::initTooltipFull(WidgetTooltip &tip, const StatBlock &stats, bool vendor_view) const {
	const char *fmt = "";
	int color;

	tip.clear();
	initTooltipName(tip, 1);

	// level
	if (level != 0) {
		tip.addLine(msg->get("Level %d", level));
	}

	// type
	if (type != ITEM_TYPE_OTHER) {
		switch (type) {
			case ITEM_TYPE_MAIN:		fmt = "Main Hand";	break;
			case ITEM_TYPE_BODY:		fmt = "Body";		break;
			case ITEM_TYPE_OFF:			fmt = "Off Hand";	break;
			case ITEM_TYPE_ARTIFACT:	fmt = "Artifact";	break;
			case ITEM_TYPE_CONSUMABLE:	fmt = "Consumable";	break;
			case ITEM_TYPE_GEM:			fmt = "Gem";		break;
			case ITEM_TYPE_QUEST:		fmt = "Quest Item";	break;
			default:					fmt = "Data error: unrecognized type";
		}
		tip.addLine(msg->get(fmt));
	}

	// damage
	if (dmg_max > 0) {
		switch (req_stat) {
			case REQUIRES_PHYS:	fmt = "Melee damage: %d";  break;
			case REQUIRES_MENT:	fmt = "Mental damage: %d"; break;
			case REQUIRES_OFF:	fmt = "Ranged damage: %d"; break;
			default:			fmt = "Data error: has dmg_max, but not req_stat %d";
		}
		tip.addLine(getRangeMsg(fmt, dmg_min, dmg_max));
	}

	// absorb
	if (abs_max > 0) {
		tip.addLine(getRangeMsg("Absorb: %d", abs_min, abs_max));
	}

	// bonuses

	string modifier;
	for(int i = 0; !bonus_stat[i].empty() && i != ITEM_MAX_BONUSES; ++i) {
		if (bonus_val[i] > 0) {
			fmt = "Increases %s by %d";
			color = FONT_GREEN;
		}
		else {
			fmt = "Decreases %s by %d";
			color = FONT_RED;
		}
		tip.addLine(msg->get(fmt, bonus_val[i], msg->get(bonus_stat[i])), color);
	}

	// power
	if (power_desc != "") {
		tip.addLine(power_desc, FONT_GREEN);
	}

	// requirement
	if (req_val > 0) {
		stringstream ss;
		ss << msg->get("Requires ") << statNames[req_stat] << " " << req_val;
		tip.addLine(ss.str(), stats.getStat(req_stat) < req_val ? FONT_RED : FONT_WHITE);
	}

	// buy or sell price
	if (price > 0) {
		const char *each = max_quantity > 1 ? " each" : "";
		if (vendor_view) {
			tip.addLine(msg->get("Buy Price: %d gold", price) + each,
						stats.gold < price ? FONT_RED : FONT_WHITE);
		}
		else {
			tip.addLine(msg->get("Sell Price: %d gold", getSellPrice()) + each);
		}
	}
}

ItemManager::~ItemManager() {

	SDL_FreeSurface(icons32);
	SDL_FreeSurface(icons64);

    if (audio == true) {
        for (int i=0; i<12; i++) {
            if (sfx[i])
                Mix_FreeChunk(sfx[i]);
        }
    }

	for (int i=0; i<MAX_ITEM_ID; i++) {
		delete[] items[i].bonus_stat;
		delete[] items[i].bonus_val;
	}

	delete[] items;

}

/**
 * Compare two item stack to be able to sorting them on their item_id in the vendors' stock
 */
bool ItemStack::operator > (const ItemStack &param) const {
	if (!item && param.item) {
		// Make the empty slots the last while sorting
		return true;
	} else if (item && !param.item == 0) {
		// Make the empty slots the last while sorting
		return false;
	} else {
		return item && param.item && (item->id > param.item->id);
	}
}

