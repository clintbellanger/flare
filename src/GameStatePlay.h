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
 * class GameStatePlay
 *
 * Handles logic and rendering of the main action game play
 * Also handles message passing between child objects, often to avoid circular dependencies.
 */

#ifndef GAMESTATEPLAY_H
#define GAMESTATEPLAY_H

#include "InputState.h"
#include "Avatar.h"
#include "Enemy.h"
#include "MapIso.h"
#include "Utils.h"
#include "HazardManager.h"
#include "EnemyManager.h"
#include "FontEngine.h"
#include "MenuManager.h"
#include "LootManager.h"
#include "PowerManager.h"
#include "NPCManager.h"
#include "CampaignManager.h"
#include "QuestLog.h"
#include "GameState.h"
#include "WidgetLabel.h"
#include "CombatText.h"

#include <SDL.h>
#include <SDL_image.h>
#include <stdexcept>

class GameStatePlay : public GameState {
private:
	PowerManager powers;
	ItemManager items;
	CampaignManager camp;
	MapIso map;
	Avatar pc;
	EnemyManager enemies;
	HazardManager hazards;
	MenuManager menu;
	LootManager loot;
	NPCManager npcs;
	QuestLog quests;
	Enemy *enemy;
	Renderable r[1024];
	int renderableCount;

	WidgetLabel label_mapname;
	WidgetLabel label_fps;

	int npc_id;
	int game_slot;

	static GameStatePlay *instance;

	bool restrictPowerUse();
	void checkEnemyFocus();
	void checkLoot();
	void checkLootDrop();
	void checkTeleport();
	void checkCancel();
	void checkLog();
	void checkEquipmentChange();
	void checkConsumable();
	void checkNotifications();
	void checkNPCInteraction();


public:
	GameStatePlay(int game_slot);
	~GameStatePlay();

	static GameStatePlay &getInstance() {
		if (!instance) {
			throw std::runtime_error("Game not in play state");
		}
		return *instance;
	}

	void logic();
	void render();
	void showFPS(int fps);
	void saveGame();
	void loadGame();
	void resetGame();

	Avatar &getPc()			{return pc;}
	MenuManager &getMenu()	{return menu;}
};

#endif

