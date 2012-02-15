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
 * class MenuManager
 */

#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Utils.h"
#include "FontEngine.h"
#include "InputState.h"
#include "MenuInventory.h"
#include "MenuPowers.h"
#include "MenuCharacter.h"
#include "MenuLog.h"
#include "MenuHUDLog.h"
#include "StatBlock.h"
#include "MenuActionBar.h"
#include "MenuHPMP.h"
#include "WidgetTooltip.h"
#include "ItemManager.h"
#include "PowerManager.h"
#include "MenuMiniMap.h"
#include "MenuExperience.h"
#include "MenuEnemy.h"
#include "MenuVendor.h"
#include "MenuTalker.h"
#include "MenuExit.h"
#include "CampaignManager.h"
#include "SharedResources.h"

const int DRAG_SRC_POWERS = 1;
const int DRAG_SRC_INVENTORY = 2;
const int DRAG_SRC_ACTIONBAR = 3;
const int DRAG_SRC_VENDOR = 4;

class MenuManager {
private:
	
	SDL_Surface *icons;

	PowerManager *powers;
	StatBlock *stats;
	CampaignManager *camp;
	
	TooltipData tip_buf;

	bool key_lock;
	void loadSounds();
	void loadIcons();
	
	bool dragging;
	ItemStack drag_stack;
	int drag_power;
	int drag_src;

	bool done;
	
public:
	MenuManager(PowerManager *powers, StatBlock *stats, CampaignManager *camp, ItemManager *items);
	~MenuManager();
	void logic();
	void render();
	void renderIcon(int icon_id, int x, int y);
	void closeAll(bool play_sound);
	void closeLeft(bool play_sound);
	void closeRight(bool play_sound);

	ItemManager *items;
	MenuInventory *inv;
	MenuPowers *pow;
	MenuCharacter *chr;
	MenuLog *log;
	MenuHUDLog *hudlog;
	MenuActionBar *act;
	MenuHPMP *hpmp;
	WidgetTooltip *tip;
	MenuMiniMap *mini;
	MenuExperience *xp;
	MenuEnemy *enemy;
	MenuVendor *vendor;
	MenuTalker *talker;
	MenuExit *exit;
	
	bool pause;
	bool menus_open;
	ItemStack drop_stack;	

	Mix_Chunk *sfx_open;
	Mix_Chunk *sfx_close;
	
	bool requestingExit() { return done; }
};

#endif
