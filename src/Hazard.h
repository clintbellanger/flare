/**
 * class Hazard
 *
 * Stand-alone object that can harm the hero or creatures
 * These are generated whenever something makes any attack
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef HAZARD_H
#define HAZARD_H

class Entity;

#include <vector>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "MapCollision.h"
#include "StatBlock.h"

class Hazard {
private:
	MapCollision *collider;
	// Keeps track of entities already hit
	std::vector<Entity*> entitiesCollided;
      
public:
	Hazard();

	StatBlock *src_stats;

	SDL_Surface *sprites;
	void setCollision(MapCollision *_collider);
	void logic();
	
      	bool hasEntity(Entity*);
	
	void addEntity(Entity*);

	//int enemyIndex; //don't know what this does... doesn't look like it's ever used.
	
	int dmg_min;
	int dmg_max;
	int crit_chance;
	int accuracy;
	
	FPoint pos;
	FPoint speed;
	int base_speed;
	int lifespan; // ticks down to zero
	int radius;
	int power_index;

	// visualization info
	bool rendered;
	Point frame_size;
	Point frame_offset;
	int frame;
	int frame_loop;
	int frame_duration;
	int active_frame; // some hazards are only dangerous on a single frame of their existence
	int direction; // some hazard animations are 8-directional
	int visual_option; // some hazard animations have random/varietal options
	bool floor; // rendererable goes on the floor layer
	int delay_frames;
	bool complete_animation; // if not multitarget but hitting a creature, still complete the animation?
	
	// these work in conjunction
	// if the attack is not multitarget, set active=false
	// only process active hazards for collision
	bool multitarget;
	bool active;
	
	bool remove_now;
	bool hit_wall;
	
	// after effects of various powers
	int stun_duration;
	int immobilize_duration;
	int slow_duration;
	int bleed_duration;
	int hp_steal;
	int mp_steal;
	
	bool trait_armor_penetration;
	int trait_crits_impaired;
	int trait_elemental;
	
	// pre/post power effects
	int post_power;
	int wall_power;
	
	bool equipment_modified;
	
};

#endif
