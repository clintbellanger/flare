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

#include "EnemyManager.h"
#include "Enemy.h"
#include "MapIso.h"
#include "ModManager.h"
#include "PowerManager.h"
#include "SharedResources.h"

#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>


using namespace std;

EnemyManager::EnemyManager(PowerManager *_powers, MapIso *_map) {
	powers = _powers;
	map = _map;
	sfx_count = 0;
	gfx_count = 0;
	hero_pos.x = hero_pos.y = -1;
	hero_alive = true;
	enemies.reserve(256);
	handleNewMap();
}


/**
 * Enemies share graphic/sound resources (usually there are groups of similar enemies)
 */
void EnemyManager::loadGraphics(const string& type_id) {

	// TODO: throw an error if a map tries to use too many monsters
	if (gfx_count == max_gfx) return;
	
	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<gfx_count; i++) {
		if (gfx_prefixes[i] == type_id) {
			return; // already have this one
		}
	}

	sprites[gfx_count].reset_and_load("images/enemies/" + type_id + ".png");
	sprites[gfx_count].set_color_key(SDL_SRCCOLORKEY, sprites[gfx_count].map_rgb(255, 0, 255)); 

	sprites[gfx_count].display_format_alpha();
	
	gfx_prefixes[gfx_count] = type_id;
	gfx_count++;

}

void EnemyManager::loadSounds(const string& type_id) {

	// TODO: throw an error if a map tries to use too many monsters
	if (sfx_count == max_sfx) return;

	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<sfx_count; i++) {
		if (sfx_prefixes[i] == type_id) {
			return; // already have this one
		}
	}

	sound_phys[sfx_count].reset_and_load("soundfx/enemies/" + type_id + "_phys.ogg");
	sound_ment[sfx_count].reset_and_load("soundfx/enemies/" + type_id + "_ment.ogg");
	sound_hit[sfx_count].reset_and_load("soundfx/enemies/" + type_id + "_hit.ogg");
	sound_die[sfx_count].reset_and_load("soundfx/enemies/" + type_id + "_die.ogg");
	sound_critdie[sfx_count].reset_and_load("soundfx/enemies/" + type_id + "_critdie.ogg");
	
	sfx_prefixes[sfx_count] = type_id;
	sfx_count++;
}

/**
 * When loading a new map, we eliminate existing enemies and load the new ones.
 * The map will have loaded Entity blocks into an array; retrieve the Enemies and init them
 */
void EnemyManager::handleNewMap () {
	
	Map_Enemy me;
	
	enemies.clear();
	
	// free shared resources
	for (int j=0; j<gfx_count; j++) {
		sprites[j].reset();
	}
	for (int j=0; j<sfx_count; j++) {
		sound_phys[j].reset();
		sound_ment[j].reset();
		sound_hit[j].reset();
		sound_die[j].reset();
		sound_critdie[j].reset();
	}
	gfx_count = 0;
	sfx_count = 0;
	
	// load new enemies
	while (!map->enemies.empty()) {
		me = map->enemies.front();
		map->enemies.pop();
		
		handleSingleSpawn(me);
	}
}

/**
 * Powers can cause new enemies to spawn
 * Check PowerManager for any new queued enemies
 */
void EnemyManager::handleSpawn() {
	
	EnemySpawn espawn;
	
	while (!powers->enemies.empty()) {
		espawn = powers->enemies.front();		
		powers->enemies.pop();	
		
		handleSingleSpawn(espawn);

		// special animation state for spawning enemies
		enemies.back().stats.cur_state = ENEMY_SPAWN;
	}
}

// TODO:  Decide whether this should take a Map_Enemy or an EnemySpawn.
template<typename T>
void EnemyManager::handleSingleSpawn(T const& me) {
	enemies.push_back(new Enemy(powers, map));
	enemies.back().stats.pos.x = me.pos.x;
	enemies.back().stats.pos.y = me.pos.y;
	enemies.back().stats.direction = me.direction;
	enemies.back().stats.load("enemies/" + me.type + ".txt");
	if (!enemies.back().stats.animations.empty()) {
		// load the animation file if specified
		enemies.back().loadAnimations("animations/" + enemies.back().stats.animations + ".txt");
	}
	else {
		cout << "Warning: no animation file specified for entity: " << me.type << endl;
	}
	loadGraphics(enemies.back().stats.gfx_prefix);
	loadSounds(enemies.back().stats.sfx_prefix);
}

/**
 * perform logic() for all enemies
 */
void EnemyManager::logic() {

	handleSpawn();

	PtrVector<Enemy>::iterator end = enemies.end();
	for (PtrVector<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
		int pref_id = -1;

		// hazards are processed after Avatar and Enemy[]
		// so process and clear sound effects from previous frames
		// check sound effects
		for (int j=0; j<sfx_count; j++) {
			if (sfx_prefixes[j] == it->stats.sfx_prefix) {
				pref_id = j;
				break;
			}
		}

		if (pref_id == -1) {
			printf("ERROR: enemy sfx_prefix doesn't match registered prefixes (enemy: '%s', sfx_prefix: '%s')\n",
			       it->stats.name.c_str(),
			       it->stats.sfx_prefix.c_str());
		} else {
			if (it->sfx_phys) sound_phys[pref_id].play_channel(-1, 0);
			if (it->sfx_ment) sound_ment[pref_id].play_channel(-1, 0);
			if (it->sfx_hit) sound_hit[pref_id].play_channel(-1, 0);
			if (it->sfx_die) sound_die[pref_id].play_channel(-1, 0);
			if (it->sfx_critdie) sound_critdie[pref_id].play_channel(-1, 0);
		}

		// clear sound flags
		it->sfx_hit = false;
		it->sfx_phys = false;
		it->sfx_ment = false;
		it->sfx_die = false;
		it->sfx_critdie = false;
		
		// new actions this round
		it->stats.hero_pos = hero_pos;
		it->stats.hero_alive = hero_alive;
		it->logic();
	}
}

Enemy* EnemyManager::enemyFocus(Point mouse, Point cam, bool alive_only) {
	Point p;
	SDL_Rect r;
	int size = enemies.size();
	PtrVector<Enemy>::iterator end = enemies.end();
	for (PtrVector<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
		if(alive_only && (it->stats.cur_state == ENEMY_DEAD || it->stats.cur_state == ENEMY_CRITDEAD)) {
			continue;
		}
		p = map_to_screen(it->stats.pos.x, it->stats.pos.y, cam.x, cam.y);
	
		r.w = it->getRender().src.w;
		r.h = it->getRender().src.h;
		r.x = p.x - it->getRender().offset.x;
		r.y = p.y - it->getRender().offset.y;
		
		if (isWithin(r, mouse)) {
			return &*it;
		}
	}
	return NULL;
}

/**
 * If an enemy has died, reward the hero with experience points
 */
void EnemyManager::checkEnemiesforXP(StatBlock *stats) {
	PtrVector<Enemy>::iterator end = enemies.end();
	for(PtrVector<Enemy>::iterator it = enemies.begin(); it != end; ++it) {
		if (it->reward_xp) {
			stats->xp += it->stats.level;
			it->reward_xp = false; // clear flag
		}
	}
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 * 
 * This wrapper function is necessary because EnemyManager holds shared sprites for identical-looking enemies
 */
Renderable EnemyManager::getRender(int enemyIndex) {
	Renderable r = enemies[enemyIndex].getRender();
	for (int i=0; i<gfx_count; i++) {
		if (gfx_prefixes[i] == enemies[enemyIndex].stats.gfx_prefix)
			r.sprite = sprites[i].get();
	}
	return r;	
}

