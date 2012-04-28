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
 * class MenuCharacter
 */

#ifndef MENU_CHARACTER_H
#define MENU_CHARACTER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "WidgetTooltip.h"
#include "InputState.h"
#include "WidgetButton.h"
#include "SharedResources.h"
#include "WidgetLabel.h"
#include <string>
#include <sstream>

enum CharStats {
	CSTAT_NAME,
	CSTAT_LEVEL,
	CSTAT_PHYSICAL,
	CSTAT_HP,
	CSTAT_HPREGEN,
	CSTAT_MENTAL,
	CSTAT_MP,
	CSTAT_MPREGEN,
	CSTAT_OFFENSE,
	CSTAT_ACCURACYV1,
	CSTAT_ACCURACYV5,
	CSTAT_DEFENSE,
	CSTAT_AVOIDANCEV1,
	CSTAT_AVOIDANCEV5,
	CSTAT_DMGMAIN,
	CSTAT_DMGRANGED,
	CSTAT_CRIT,
	CSTAT_ABSORB,
	CSTAT_FIRERESIST,
	CSTAT_ICERESIST,
	CSTAT_COUNT
};

enum CharProficiencies {
	CPROF_P2,
	CPROF_P3,
	CPROF_P4,
	CPROF_P5,
	CPROF_M2,
	CPROF_M3,
	CPROF_M4,
	CPROF_M5,
	CPROF_O2,
	CPROF_O3,
	CPROF_O4,
	CPROF_O5,
	CPROF_D2,
	CPROF_D3,
	CPROF_D4,
	CPROF_D5,
	CPROF_COUNT
};

static const char *charProfDesc[CPROF_COUNT] = {
	"Dagger",
	"Shortsword",
	"Longsword",
	"Greatsword",
	"Wand",
	"Rod",
	"Staff",
	"Greatstaff",
	"Slingshot",
	"Shortbow",
	"Longbow",
	"Greatbow",
	"Light Armor",
	"Light Shield",
	"Heavy Armor",
	"Heavy Shield"
};

struct CharStat : private Uncopyable {
	WidgetLabel *label;
	WidgetLabel *value;
	SDL_Rect hover;
	WidgetTooltip tip;

	void setHover(int x, int y, int w, int h) {
		hover.x=x;
		hover.y=y;
		hover.w=w;
		hover.h=h;
	}
};

struct CharProf {
	SDL_Rect hover;
	WidgetTooltip tip;

	void setHover(int x, int y, int w, int h) {
		hover.x=x;
		hover.y=y;
		hover.w=w;
		hover.h=h;
	}
};

class MenuCharacter : private Uncopyable {
private:

	StatBlock &stats;

	SDL_Surface *background;
	SDL_Surface *proficiency;
	SDL_Surface *upgrade;
	WidgetButton *closeButton;
	WidgetLabel *labelCharacter;
	CharStat cstat[CSTAT_COUNT];
	CharProf cprof[CPROF_COUNT];

	void initProfTips();
	void displayProficiencies(int value, int y);
	void loadGraphics();
	int bonusColor(int stat);

public:
	MenuCharacter(StatBlock &stats);
	~MenuCharacter();
	void logic();
	void render();
	void refreshStats();
	WidgetTooltip *checkTooltip();
	bool checkUpgrade();

	bool visible;
    bool newPowerNotification;

};

#endif
