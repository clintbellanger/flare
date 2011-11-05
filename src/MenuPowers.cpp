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
 * class MenuPowers
 */

#include "MenuPowers.h"

MenuPowers::MenuPowers(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats, PowerManager *_powers) {
	screen = _screen;
	inp = _inp;
	font = _font;
	stats = _stats;
	powers = _powers;
	
	visible = false;
	loadGraphics();
	
			
	// set slot positions
	int offset_x = (VIEW_W - 320);
	int offset_y = (VIEW_H - 416)/2;

	for (int i=0; i<20; i++) {
		slots[i].w = slots[i].h = 32;
		slots[i].x = offset_x + 48 + (i % 4) * 64;
		slots[i].y = offset_y + 80 + (i / 4) * 64;
	}
	
	closeButton = new WidgetButton(screen, font, inp, "images/menus/buttons/button_x.png");
	closeButton->pos.x = VIEW_W - 26;
	closeButton->pos.y = (VIEW_H - 480)/2 + 34;

}

void MenuPowers::loadGraphics() {

	background = IMG_Load((PATH_DATA + "images/menus/powers.png").c_str());
	powers_step = IMG_Load((PATH_DATA + "images/menus/powers_step.png").c_str());
	powers_unlock = IMG_Load((PATH_DATA + "images/menus/powers_unlock.png").c_str()); 
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
			if (requirementsMet(i)) return i;
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
	
	int offset_x = (VIEW_W - 320);
	int offset_y = (VIEW_H - 416)/2;
	
	// background
	src.x = 0;
	src.y = 0;
	dest.x = offset_x;
	dest.y = offset_y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// close button
	closeButton->render();
	
	// text overlay
	font->render(msg->get("Powers"), offset_x+160, offset_y+8, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Physical"), offset_x+64, offset_y+50, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Physical"), offset_x+128, offset_y+50, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Mental"), offset_x+192, offset_y+50, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Mental"), offset_x+256, offset_y+50, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Offense"), offset_x+64, offset_y+66, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Defense"), offset_x+128, offset_y+66, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Offense"), offset_x+192, offset_y+66, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("Defense"), offset_x+256, offset_y+66, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	// stats
	stringstream ss;
	ss.str("");
	ss << stats->physoff;
	font->render(ss.str(), offset_x+64, offset_y+34, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->physdef;
	font->render(ss.str(), offset_x+128, offset_y+34, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->mentoff;
	font->render(ss.str(), offset_x+192, offset_y+34, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->mentdef;
	font->render(ss.str(), offset_x+256, offset_y+34, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	// highlighting
	displayBuild(stats->physoff, offset_x+48);
	displayBuild(stats->physdef, offset_x+112);
	displayBuild(stats->mentoff, offset_x+176);
	displayBuild(stats->mentdef, offset_x+240);	
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
	int offset_y = (VIEW_H - 416)/2;
	
	// save-game hackers could set their stats higher than normal.
	// make sure this display still works.
	int display_value = min(value,10);
	
	for (int i=3; i<= display_value; i++) {
		if (i%2 == 0) { // even stat
			dest.y = i * 32 + offset_y + 48;
			SDL_BlitSurface(powers_step, &src_step, screen, &dest);
		}
		else { // odd stat
			dest.y = i * 32 + offset_y + 35;
			SDL_BlitSurface(powers_unlock, &src_unlock, screen, &dest);
		
		}
	}
}

/**
 * Show mouseover descriptions of disciplines and powers
 */
TooltipData MenuPowers::checkTooltip(Point mouse) {

	TooltipData tip;

	int offset_x = (VIEW_W - 320);
	int offset_y = (VIEW_H - 416)/2;
	
	if (mouse.y >= offset_y+32 && mouse.y <= offset_y+80) {
		if (mouse.x >= offset_x+48 && mouse.x <= offset_x+80) {
			tip.lines[tip.num_lines++] = msg->get("Physical + Offense grants melee and ranged attacks");
			return tip;
		}
		if (mouse.x >= offset_x+112 && mouse.x <= offset_x+144) {
			tip.lines[tip.num_lines++] = msg->get("Physical + Defense grants melee protection");
			return tip;
		}
		if (mouse.x >= offset_x+176 && mouse.x <= offset_x+208) {
			tip.lines[tip.num_lines++] = msg->get("Mental + Offense grants elemental spell attacks");
			return tip;
		}
		if (mouse.x >= offset_x+240 && mouse.x <= offset_x+272) {
			tip.lines[tip.num_lines++] = msg->get("Mental + Defense grants healing and magical protection");
			return tip;
		}
	}
	else {
		for (int i=0; i<20; i++) {
			if (isWithin(slots[i], mouse)) {
				tip.lines[tip.num_lines++] = powers->powers[i].name;
				tip.lines[tip.num_lines++] = powers->powers[i].description;
				
				if (powers->powers[i].requires_physical_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a physical weapon");
				else if (powers->powers[i].requires_mental_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a mental weapon");
				else if (powers->powers[i].requires_offense_weapon)
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
				if (powers->powers[i].requires_mp > 0) {
					tip.lines[tip.num_lines++] = msg->get("Costs %d MP", powers->powers[i].requires_mp);
				}
				// add cooldown time
				if (powers->powers[i].cooldown > 0) {
					tip.lines[tip.num_lines++] = msg->get("Cooldown: %d seconds", powers->powers[i].cooldown / 1000.0);
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
