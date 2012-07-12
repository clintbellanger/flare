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
 * class MenuPowers
 */

#include "Menu.h"
#include "FileParser.h"
#include "MenuPowers.h"
#include "SharedResources.h"
#include "PowerManager.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetLabel.h"
#include "WidgetTooltip.h"

#include <string>
#include <sstream>

using namespace std;


MenuPowers::MenuPowers(StatBlock *_stats, PowerManager *_powers, SDL_Surface *_icons) {
	stats = _stats;
	powers = _powers;
	icons = _icons;

	visible = false;
	loadGraphics();

	for (int i=0; i<20; i++) {
		power_ui[i].id = 0;
		power_ui[i].pos.x = 0;
		power_ui[i].pos.y = 0;
	}

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Read powers data from config file 
	FileParser infile;
	int counter = -1;
	if (infile.open(mods->locate("menus/powers.txt"))) {
	  while (infile.next()) {
		infile.val = infile.val + ',';

		if (infile.key == "id") {
			counter++;
			power_ui[counter].id = eatFirstInt(infile.val, ',');
		} else if (infile.key == "position") {
			power_ui[counter].pos.x = eatFirstInt(infile.val, ',');
			power_ui[counter].pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "closebutton_pos") {
			close_pos.x = eatFirstInt(infile.val, ',');
			close_pos.y = eatFirstInt(infile.val, ',');
		}

	  }
	} else fprintf(stderr, "Unable to open powers_menu.txt!\n");
	infile.close();

}

void MenuPowers::update() {
	for (int i=0; i<20; i++) {
		slots[i].w = slots[i].h = 32;
		slots[i].x = window_area.x + power_ui[i].pos.x;
		slots[i].y = window_area.y + power_ui[i].pos.y;
	}

	label_powers.set(window_area.x+160, window_area.y+8, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Powers"), FONT_WHITE);
	label_p1.set(window_area.x+64, window_area.y+50, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Physical"), FONT_WHITE);
	label_p2.set(window_area.x+128, window_area.y+50, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Physical"), FONT_WHITE);
	label_m1.set(window_area.x+192, window_area.y+50, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Mental"), FONT_WHITE);
	label_m2.set(window_area.x+256, window_area.y+50, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Mental"), FONT_WHITE);
	label_o1.set(window_area.x+64, window_area.y+66, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Offense"), FONT_WHITE);
	label_o2.set(window_area.x+192, window_area.y+66, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Offense"), FONT_WHITE);
	label_d1.set(window_area.x+128, window_area.y+66, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Defense"), FONT_WHITE);
	label_d2.set(window_area.x+256, window_area.y+66, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Defense"), FONT_WHITE);

	stat_po.set(window_area.x+64, window_area.y+34, JUSTIFY_CENTER, VALIGN_TOP, "", FONT_WHITE);
	stat_pd.set(window_area.x+128, window_area.y+34, JUSTIFY_CENTER, VALIGN_TOP, "", FONT_WHITE);
	stat_mo.set(window_area.x+192, window_area.y+34, JUSTIFY_CENTER, VALIGN_TOP, "", FONT_WHITE);
	stat_md.set(window_area.x+256, window_area.y+34, JUSTIFY_CENTER, VALIGN_TOP, "", FONT_WHITE);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuPowers::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/powers.png").c_str());
	powers_step = IMG_Load(mods->locate("images/menus/powers_step.png").c_str());
	powers_unlock = IMG_Load(mods->locate("images/menus/powers_unlock.png").c_str());
	if(!background || !powers_step || !powers_unlock) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = powers_step;
	powers_step = SDL_DisplayFormatAlpha(powers_step);
	SDL_FreeSurface(cleanup);

	cleanup = powers_unlock;
	powers_unlock = SDL_DisplayFormatAlpha(powers_unlock);
	SDL_FreeSurface(cleanup);
}

/**
 * generic render 32-pixel icon
 */
void MenuPowers::renderIcon(int icon_id, int x, int y) {
	SDL_Rect icon_src;
	SDL_Rect icon_dest;

	icon_dest.x = x;
	icon_dest.y = y;
	icon_src.w = icon_src.h = icon_dest.w = icon_dest.h = 32;
	icon_src.x = (icon_id % 16) * 32;
	icon_src.y = (icon_id / 16) * 32;
	SDL_BlitSurface(icons, &icon_src, screen, &icon_dest);
}

/**
 * With great power comes great stat requirements.
 */
bool MenuPowers::requirementsMet(int power_index) {
	int required_val = (power_index / 4) * 2 + 1;
	int required_stat = power_index % 4;
	switch (required_stat) {
		case 0:
			return (stats->physoff >= required_val);
			break;
		case 1:
			return (stats->physdef >= required_val);
			break;
		case 2:
			return (stats->mentoff >= required_val);
			break;
		case 3:
			return (stats->mentdef >= required_val);
			break;
	}
	return false;
}

/**
 * Click-to-drag a power (to the action bar)
 */
int MenuPowers::click(Point mouse) {

	for (int i=0; i<20; i++) {
		if (isWithin(slots[i], mouse)) {
			if (requirementsMet(power_ui[i].id)) return power_ui[i].id;
			else return -1;
		}
	}
	return -1;
}


void MenuPowers::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

void MenuPowers::render() {
	if (!visible) return;

	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);

	// power icons
	for (int i=0; i<20; i++) {
		renderIcon(powers->powers[power_ui[i].id].icon, window_area.x + power_ui[i].pos.x, window_area.y + power_ui[i].pos.y);
	}

	// close button
	closeButton->render();

	// text overlay
	label_powers.render();
	label_p1.render();
	label_p2.render();
	label_m1.render();
	label_m2.render();
	label_o1.render();
	label_o2.render();
	label_d1.render();
	label_d2.render();

	// stats
	stringstream ss;

	ss.str("");
	ss << stats->physoff;
	stat_po.set(ss.str());
	stat_po.render();

	ss.str("");
	ss << stats->physdef;
	stat_pd.set(ss.str());
	stat_pd.render();

	ss.str("");
	ss << stats->mentoff;
	stat_mo.set(ss.str());
	stat_mo.render();

	ss.str("");
	ss << stats->mentdef;
	stat_md.set(ss.str());
	stat_md.render();

	// highlighting
	displayBuild(stats->physoff, window_area.x+48);
	displayBuild(stats->physdef, window_area.x+112);
	displayBuild(stats->mentoff, window_area.x+176);
	displayBuild(stats->mentdef, window_area.x+240);
}

/**
 * Highlight unlocked powers
 */
void MenuPowers::displayBuild(int value, int x) {
	SDL_Rect src_step;
	SDL_Rect src_unlock;
	SDL_Rect dest;

	src_step.x = src_unlock.x = 0;
	src_step.y = src_unlock.y = 0;
	src_step.w = 32;
	src_step.h = 19;
	src_unlock.w = 32;
	src_unlock.h = 45;

	dest.x = x;

	// save-game hackers could set their stats higher than normal.
	// make sure this display still works.
	int display_value = min(value,10);

	for (int i=3; i<= display_value; i++) {
		if (i%2 == 0) { // even stat
			dest.y = i * 32 + window_area.y + 48;
			SDL_BlitSurface(powers_step, &src_step, screen, &dest);
		}
		else { // odd stat
			dest.y = i * 32 + window_area.y + 35;
			SDL_BlitSurface(powers_unlock, &src_unlock, screen, &dest);

		}
	}
}

/**
 * Show mouseover descriptions of disciplines and powers
 */
TooltipData MenuPowers::checkTooltip(Point mouse) {

	TooltipData tip;

	if (mouse.y >= window_area.y+32 && mouse.y <= window_area.y+80) {
		if (mouse.x >= window_area.x+48 && mouse.x <= window_area.x+80) {
			tip.lines[tip.num_lines++] = msg->get("Physical + Offense grants melee and ranged attacks");
			return tip;
		}
		if (mouse.x >= window_area.x+112 && mouse.x <= window_area.x+144) {
			tip.lines[tip.num_lines++] = msg->get("Physical + Defense grants melee protection");
			return tip;
		}
		if (mouse.x >= window_area.x+176 && mouse.x <= window_area.x+208) {
			tip.lines[tip.num_lines++] = msg->get("Mental + Offense grants elemental spell attacks");
			return tip;
		}
		if (mouse.x >= window_area.x+240 && mouse.x <= window_area.x+272) {
			tip.lines[tip.num_lines++] = msg->get("Mental + Defense grants healing and magical protection");
			return tip;
		}
	}
	else {
		for (int i=0; i<20; i++) {
			if (isWithin(slots[i], mouse)) {
				tip.lines[tip.num_lines++] = powers->powers[power_ui[i].id].name;
				tip.lines[tip.num_lines++] = powers->powers[power_ui[i].id].description;

				if (powers->powers[power_ui[i].id].requires_physical_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a physical weapon");
				else if (powers->powers[power_ui[i].id].requires_mental_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a mental weapon");
				else if (powers->powers[power_ui[i].id].requires_offense_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires an offense weapon");


				// add requirement
				int required_val = (i / 4) * 2 + 1;
				int required_stat = i % 4;
				if (required_val > 1) {

					if (!requirementsMet(i))
						tip.colors[tip.num_lines] = FONT_RED;

					if (required_stat == 0) tip.lines[tip.num_lines++] = msg->get("Requires Physical Offense %d", required_val);
					else if (required_stat == 1) tip.lines[tip.num_lines++] = msg->get("Requires Physical Defense %d", required_val);
					else if (required_stat == 2) tip.lines[tip.num_lines++] = msg->get("Requires Mental Offense %d", required_val);
					else tip.lines[tip.num_lines++] = msg->get("Requires Mental Defense %d", required_val);

				}

				// add mana cost
				if (powers->powers[power_ui[i].id].requires_mp > 0) {
					tip.lines[tip.num_lines++] = msg->get("Costs %d MP", powers->powers[power_ui[i].id].requires_mp);
				}
				// add cooldown time
				if (powers->powers[power_ui[i].id].cooldown > 0) {
					tip.lines[tip.num_lines++] = msg->get("Cooldown: %d seconds", powers->powers[power_ui[i].id].cooldown / 1000.0);
				}

				return tip;
			}
		}
	}

	return tip;
}

MenuPowers::~MenuPowers() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(powers_step);
	SDL_FreeSurface(powers_unlock);
	delete closeButton;
}
