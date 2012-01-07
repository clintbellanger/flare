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

/*
 * class EnemyManager
 */
 
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "PtrVector.h"
#include "ScopedPtr.h"
#include "SmartChunk.h"
#include "SmartSurface.h"
#include "Utils.h"

class Enemy;
class Map_Enemy;
class MapIso;
class PowerManager;
class Renderable;
class StatBlock;

// TODO: rename these to something more specific to EnemyManager
const int max_sfx = 8;
const int max_gfx = 32;

class EnemyManager {
private:

	MapIso *map;
	PowerManager *powers;
	void loadGraphics(const std::string& type_id);
	void loadSounds(const std::string& type_id);
	template<typename T>
	void handleSingleSpawn(T const& me);

	std::string gfx_prefixes[max_gfx];
	int gfx_count;
	std::string sfx_prefixes[max_sfx];
	int sfx_count;
	
	SmartSurface sprites[max_gfx];	
	SmartChunk sound_phys[max_sfx];
	SmartChunk sound_ment[max_sfx];
	SmartChunk sound_hit[max_sfx];
	SmartChunk sound_die[max_sfx];
	SmartChunk sound_critdie[max_sfx];
	
public:
	EnemyManager(PowerManager *_powers, MapIso *_map);

	void handleNewMap();
	void handleSpawn();
	void logic();
	Renderable getRender(int enemyIndex);
	void checkEnemiesforXP(StatBlock *stats);
	Enemy *enemyFocus(Point mouse, Point cam, bool alive_only);

	// vars
	PtrVector<Enemy> enemies;
	Point hero_pos;
	bool hero_alive;
};


#endif
