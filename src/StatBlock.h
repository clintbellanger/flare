/**
 * class StatBlock
 *
 * Character stats and calculations
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef STAT_BLOCK_H
#define STAT_BLOCK_H

#include <string>
#include <fstream>
#include "Settings.h"
#include "Utils.h"
#include "MessageEngine.h"

using namespace std;

const int STAT_EFFECT_SHIELD = 0;
const int STAT_EFFECT_VENGEANCE = 1;

const int POWERSLOT_COUNT = 5;
const int MELEE_PHYS = 0;
const int MELEE_MENT = 1;
const int RANGED_PHYS = 2;
const int RANGED_MENT = 3;
const int BEACON = 4;

const int MAX_CHARACTER_LEVEL = 32;

class StatBlock {
private:

public:
	StatBlock();
	~StatBlock();

	MessageEngine *msg;

	void load(string filename);
	void takeDamage(int dmg);
	void recalc();
	void logic();
	void clearEffects();
	Renderable getEffectRender(int effect_type);

	bool alive;
	bool corpse; // creature is dead and done animating
	bool hero; // else, enemy or other
	
	string name;
	string sfx_prefix;
	string gfx_prefix;
	
	int level;
	int xp;
	int xp_table[MAX_CHARACTER_LEVEL+1];

	// base stats ("attributes")
	int offense_character;
	int defense_character;
	int physical_character;
	int mental_character;

	// additional values to base stats, given by items
	int offense_additional;
	int defense_additional;
	int physical_additional;
	int mental_additional;

	// getters for full base stats (character + additional)
	inline int get_offense() { return offense_character + offense_additional; }
	inline int get_defense() { return defense_character + defense_additional; }
	inline int get_physical() { return physical_character + physical_additional; }
	inline int get_mental() { return mental_character + mental_additional; }

	// derived stats ("disciplines")
	int physoff;
	int physdef;
	int mentoff;
	int mentdef;
	int physment;
	int offdef;
	
	// in Flare there are no distinct character classes.
	// instead each class is given a descriptor based on their base stat builds
	string character_class;
	
	// physical stats
	int hp;
	int maxhp;
	int hp_per_minute;
	int hp_ticker;

	// mental stats
	int mp;
	int maxmp;
	int mp_per_minute;
	int mp_ticker;
	
	// offense stats
	int accuracy;
	
	// defense stats
	int avoidance;

	// overall stats
	int crit;

	// equipment stats	
	int dmg_melee_min;
	int dmg_melee_max;
	int dmg_ment_min;
	int dmg_ment_max;
	int dmg_ranged_min;
	int dmg_ranged_max;
	int absorb_min;
	int absorb_max;	
	bool wielding_physical;
	bool wielding_mental;
	bool wielding_offense;
	bool ammo_arrows;
	int attunement_fire;
	int attunement_ice;

	// buff and debuff stats
	int slow_duration;
	int bleed_duration;
	int stun_duration;
	int immobilize_duration;
	int immunity_duration;	
	int haste_duration;
	int hot_duration;
	int hot_value;
	
	int shield_hp; // shield
	int shield_frame;
	bool blocking;
	int vengeance_stacks;
	int vengeance_frame;
	
	int speed;
	int dspeed;
	Point pos;
	int direction;
	int hero_cooldown[1024]; //TODO: fix this to use POWER_COUNT... right now it can't #include "PowerManager.h"
		
	// state
	int cur_state;

	// enemy behavioral stats
	int chance_pursue;
	int chance_flee;
	
	int power_chance[POWERSLOT_COUNT];
	int power_index[POWERSLOT_COUNT];
	int power_cooldown[POWERSLOT_COUNT];
	int power_ticks[POWERSLOT_COUNT];
		
	int melee_range;
	int threat_range;
	Point hero_pos;
	bool hero_alive;
	Point last_seen; 
	int dir_favor;
	int dir_ticks;
	int patrol_ticks;
	bool in_combat;
	int cooldown_ticks;
	int cooldown; // min. # of frames between abilities
	
	int loot_chance;
	
	// for the teleport spell
	bool teleportation;
	Point teleport_destination;
	
	// weapons can modify spells
	int melee_weapon_power;
	int mental_weapon_power;
	int ranged_weapon_power;
	
	// for purchasing tracking
	int gold;
	
	// marked for death
	bool death_penalty;
	
	// Campaign event interaction
	string defeat_status;
	string quest_loot_requires;
	string quest_loot_not;
	int quest_loot_id;
	int first_defeat_loot;
	
	// player look options
	string base; // folder in /images/avatar
	string head; // png in /images/avatar/[base]
	string portrait; // png in /images/portraits

	string animations;
	int animationSpeed;
};

#endif

