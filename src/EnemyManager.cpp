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

#include "EnemyManager.h"
#include "SharedResources.h"
#include "EnemyBehavior.h"
#include "BehaviorStandard.h"

#include <iostream>

using namespace std;

EnemyManager::EnemyManager(PowerManager *_powers, MapRenderer *_map) {
	powers = _powers;
	map = _map;
	enemies = vector<Enemy*>();
	sfx_count = 0;
	gfx_count = 0;
	hero_pos.x = hero_pos.y = -1;
	hero_alive = true;
	handleNewMap();
}



/**
 * Enemies share graphic/sound resources (usually there are groups of similar enemies)
 */
bool EnemyManager::loadGraphics(const string& type_id) {

	if (gfx_count == max_enemy_gfx) return false;

	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<gfx_count; i++) {
		if (gfx_prefixes[i] == type_id) {
			return true; // already have this one
		}
	}

	sprites[gfx_count] = IMG_Load(mods->locate("images/enemies/" + type_id + ".png").c_str());
	if(!sprites[gfx_count]) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		return false;
	}
	SDL_SetColorKey( sprites[gfx_count], SDL_SRCCOLORKEY, SDL_MapRGB(sprites[gfx_count]->format, 255, 0, 255) );

	// optimize
	SDL_Surface *cleanup = sprites[gfx_count];
	sprites[gfx_count] = SDL_DisplayFormatAlpha(sprites[gfx_count]);
	SDL_FreeSurface(cleanup);

	gfx_prefixes[gfx_count] = type_id;
	gfx_count++;

	return true;
}

bool EnemyManager::loadSounds(const string& type_id) {

    if (sfx_count == max_enemy_sfx) return false;

    // first check to make sure the sprite isn't already loaded
    for (int i=0; i<sfx_count; i++) {
        if (sfx_prefixes[i] == type_id) {
            return true; // already have this one
        }
    }

    if (audio && SOUND_VOLUME) {
        sound_phys[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_phys.ogg").c_str());
        sound_ment[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_ment.ogg").c_str());
        sound_hit[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_hit.ogg").c_str());
        sound_die[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_die.ogg").c_str());
        sound_critdie[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_critdie.ogg").c_str());
    } else {
        sound_phys[sfx_count] = NULL;
        sound_ment[sfx_count] = NULL;
        sound_hit[sfx_count] = NULL;
        sound_die[sfx_count] = NULL;
        sound_critdie[sfx_count] = NULL;
    }

    sfx_prefixes[sfx_count] = type_id;
    sfx_count++;

	return true;
}

/**
 * When loading a new map, we eliminate existing enemies and load the new ones.
 * The map will have loaded Entity blocks into an array; retrieve the Enemies and init them
 */
void EnemyManager::handleNewMap () {

	Map_Enemy me;

	// delete existing enemies
	for (unsigned int i=0; i < enemies.size(); i++) {
		delete enemies[i];
	}
	enemies.clear();

	// free shared resources
	for (int j=0; j<gfx_count; j++) {
		SDL_FreeSurface(sprites[j]);
	}
	for (int j=0; j<sfx_count; j++) {
		Mix_FreeChunk(sound_phys[j]);
		Mix_FreeChunk(sound_ment[j]);
		Mix_FreeChunk(sound_hit[j]);
		Mix_FreeChunk(sound_die[j]);
		Mix_FreeChunk(sound_critdie[j]);
	}
	gfx_count = 0;
	sfx_count = 0;

	// load new enemies
	while (!map->enemies.empty()) {
		me = map->enemies.front();
		map->enemies.pop();

		Enemy *e = new Enemy(powers, map);

		// factory
		e->eb = new BehaviorStandard(e);

		e->stats.waypoints = me.waypoints;
		e->stats.pos.x = me.pos.x;
		e->stats.pos.y = me.pos.y;
		e->stats.direction = me.direction;
		e->stats.wander = me.wander;
		e->stats.wander_area = me.wander_area;
		e->stats.load("enemies/" + me.type + ".txt");
		if (e->stats.animations != "") {
			// load the animation file if specified
			e->loadAnimations("animations/" + e->stats.animations + ".txt");
		}
		else {
			cerr << "Warning: no animation file specified for entity: " << me.type << endl;
		}
		if (!loadGraphics(e->stats.gfx_prefix)) {
			cerr << "Warning: could not load graphics prefix: " << e->stats.gfx_prefix << endl;
			continue;
		}
		if (!loadSounds(e->stats.sfx_prefix)) {
			cerr << "Warning: could not load sounds prefix: " << e->stats.sfx_prefix << endl;
			continue;
		}
		enemies.push_back(e);
	}
}

/**
 * Powers can cause new enemies to spawn
 * Check PowerManager for any new queued enemies
 */
void EnemyManager::handleSpawn() {

	Map_Enemy espawn;

	while (!powers->enemies.empty()) {
		espawn = powers->enemies.front();
		powers->enemies.pop();

		Enemy *e = new Enemy(powers, map);
		// factory
		e->eb = new BehaviorStandard(e);

		e->stats.pos.x = espawn.pos.x;
		e->stats.pos.y = espawn.pos.y;
		e->stats.direction = espawn.direction;
		e->stats.load("enemies/" + espawn.type + ".txt");
		if (e->stats.animations != "") {
			// load the animation file if specified
			e->loadAnimations("animations/" + e->stats.animations + ".txt");
		}
		else {
			cout << "Warning: no animation file specified for entity: " << espawn.type << endl;
		}
		loadGraphics(e->stats.gfx_prefix);
		loadSounds(e->stats.sfx_prefix);

		// special animation state for spawning enemies
		e->stats.cur_state = ENEMY_SPAWN;
		enemies.push_back(e);
	}
}

/**
 * perform logic() for all enemies
 */
void EnemyManager::logic() {

	handleSpawn();

	for (unsigned int i=0; i < enemies.size(); i++) {

		int pref_id = -1;

		// hazards are processed after Avatar and Enemy[]
		// so process and clear sound effects from previous frames
		// check sound effects
		if (audio == true) {
			for (int j=0; j<sfx_count; j++) {
				if (sfx_prefixes[j] == enemies[i]->stats.sfx_prefix) {
					pref_id = j;
					break;
				}
			}

			if (pref_id == -1) {
				printf("ERROR: enemy sfx_prefix doesn't match registered prefixes (enemy: '%s', sfx_prefix: '%s')\n",
					   enemies[i]->stats.name.c_str(),
					   enemies[i]->stats.sfx_prefix.c_str());
			} else {
				if (enemies[i]->sfx_phys) Mix_PlayChannel(-1, sound_phys[pref_id], 0);
				if (enemies[i]->sfx_ment) Mix_PlayChannel(-1, sound_ment[pref_id], 0);
				if (enemies[i]->sfx_hit) Mix_PlayChannel(-1, sound_hit[pref_id], 0);
				if (enemies[i]->sfx_die) Mix_PlayChannel(-1, sound_die[pref_id], 0);
				if (enemies[i]->sfx_critdie) Mix_PlayChannel(-1, sound_critdie[pref_id], 0);
			}

			// clear sound flags
			enemies[i]->sfx_hit = false;
			enemies[i]->sfx_phys = false;
			enemies[i]->sfx_ment = false;
			enemies[i]->sfx_die = false;
			enemies[i]->sfx_critdie = false;
		}

		// new actions this round
		enemies[i]->stats.hero_pos = hero_pos;
		enemies[i]->stats.hero_alive = hero_alive;
		enemies[i]->logic();
	}
}

Enemy* EnemyManager::enemyFocus(Point mouse, Point cam, bool alive_only) {
	Point p;
	SDL_Rect r;
	for(unsigned int i = 0; i < enemies.size(); i++) {
		if(alive_only && (enemies[i]->stats.cur_state == ENEMY_DEAD || enemies[i]->stats.cur_state == ENEMY_CRITDEAD)) {
			continue;
		}
		p = map_to_screen(enemies[i]->stats.pos.x, enemies[i]->stats.pos.y, cam.x, cam.y);

		r.w = enemies[i]->getRender().src.w;
		r.h = enemies[i]->getRender().src.h;
		r.x = p.x - enemies[i]->getRender().offset.x;
		r.y = p.y - enemies[i]->getRender().offset.y;

		if (isWithin(r, mouse)) {
			Enemy *enemy = enemies[i];
			return enemy;
		}
	}
	return NULL;
}

/**
 * If an enemy has died, reward the hero with experience points
 */
void EnemyManager::checkEnemiesforXP(StatBlock *stats) {
	for (unsigned int i=0; i < enemies.size(); i++) {
		if (enemies[i]->reward_xp) {
			stats->xp += enemies[i]->stats.xp;
			enemies[i]->reward_xp = false; // clear flag
		}
	}
}

/**
 * addRenders()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 *
 * This wrapper function is necessary because EnemyManager holds shared sprites for identical-looking enemies
 */
void EnemyManager::addRenders(vector<Renderable> &r, vector<Renderable> &r_dead) {
	vector<Enemy*>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); it++) {
		Renderable re = (*it)->getRender();
		for (int i=0; i<gfx_count; i++) {
			if (gfx_prefixes[i] == (*it)->stats.gfx_prefix)
				re.sprite = sprites[i];
		}
		// draw corpses below objects so that floor loot is more visible
		bool dead = (*it)->stats.corpse;
		(dead ? r_dead : r).push_back(re);
		if ((*it)->stats.shield_hp > 0) {
			re = (*it)->stats.getEffectRender(STAT_EFFECT_SHIELD);
			re.sprite = powers->gfx[powers->powers[POWER_SHIELD].gfx_index]; // TODO: parameter
			(dead ? r_dead : r).push_back(re);
		}
	}
}

EnemyManager::~EnemyManager() {
	for (unsigned int i=0; i < enemies.size(); i++) {
		delete enemies[i];
	}

	for (int i=0; i<gfx_count; i++) {
		SDL_FreeSurface(sprites[i]);
	}
	for (int i=0; i<sfx_count; i++) {
		Mix_FreeChunk(sound_phys[i]);
		Mix_FreeChunk(sound_ment[i]);
		Mix_FreeChunk(sound_hit[i]);
		Mix_FreeChunk(sound_die[i]);
		Mix_FreeChunk(sound_critdie[i]);
	}
}
