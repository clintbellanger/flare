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

/*
 * class EnemyManager
 */

#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "Settings.h"
#include "MapRenderer.h"
#include "Enemy.h"
#include "Utils.h"
#include "PowerManager.h"

const int max_enemy_sfx = 8;
const int max_enemy_gfx = 32;

class EnemyManager {
private:

	MapRenderer *map;
	PowerManager *powers;
	bool loadGraphics(const std::string& type_id);
	bool loadSounds(const std::string& type_id);

	std::string gfx_prefixes[max_enemy_gfx];
	int gfx_count;
	std::string sfx_prefixes[max_enemy_sfx];
	int sfx_count;

	SDL_Surface *sprites[max_enemy_gfx];
	Mix_Chunk *sound_phys[max_enemy_sfx];
	Mix_Chunk *sound_ment[max_enemy_sfx];
	Mix_Chunk *sound_hit[max_enemy_sfx];
	Mix_Chunk *sound_die[max_enemy_sfx];
	Mix_Chunk *sound_critdie[max_enemy_sfx];

public:
	EnemyManager(PowerManager *_powers, MapRenderer *_map);
	~EnemyManager();
	void handleNewMap();
	void handleSpawn();
	void logic();
	void addRenders(std::vector<Renderable> &r, std::vector<Renderable> &r_dead);
	void checkEnemiesforXP(StatBlock *stats);
	Enemy *enemyFocus(Point mouse, Point cam, bool alive_only);

	// vars
	std::vector<Enemy*> enemies;
	Point hero_pos;
	bool hero_alive;
};


#endif
