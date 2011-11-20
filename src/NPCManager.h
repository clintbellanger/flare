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
 * class NPCManager
 *
 * NPCs which are not combatative enemies are handled by this Manager.
 * Most commonly this involves vendor and conversation townspeople.
 */

#ifndef NPC_MANAGER_H
#define NPC_MANAGER_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "NPC.h"
#include "MapIso.h"
#include "MenuTooltip.h"
#include "LootManager.h"

using namespace std;

// max number of NPCs for a single map
const int MAX_NPC_COUNT = 32;

class NPCManager {
private:
	MapIso *map;
	MenuTooltip *tip;
	LootManager *loot;
	ItemDatabase *items;
public:
	NPCManager(MapIso *_map, MenuTooltip *_tip, LootManager *_loot, ItemDatabase *_items);
	~NPCManager();
	NPC *npcs[MAX_NPC_COUNT];
	void handleNewMap();
	void logic();
	int checkNPCClick(Point mouse, Point cam);
	void renderTooltips(Point cam, Point mouse);
	int npc_count;
	int tooltip_margin;
};

#endif
