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
 * GameStateNew
 * 
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 */

#include "GameStateNew.h"
#include "GameStateLoad.h"
#include "GameStatePlay.h"
#include "ModManager.h"

GameStateNew::GameStateNew(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : GameState(_screen, _inp, _font) {
	game_slot = 0;
	option_count = 0;
	current_option = 0;
	portrait_image = NULL;
	
	button_exit = new WidgetButton(screen, font, inp, mods->locate("images/menus/buttons/button_default.png"));
	button_exit->label = msg->get("Cancel");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	
	button_create = new WidgetButton(screen, font, inp, mods->locate("images/menus/buttons/button_default.png"));
	button_create->label = msg->get("Create Character");
	button_create->pos.x = VIEW_W_HALF + button_create->pos.w/2;
	button_create->pos.y = VIEW_H - button_create->pos.h;

	button_prev = new WidgetButton(screen, font, inp, mods->locate("images/menus/buttons/left.png"));
	button_prev->pos.x = VIEW_W_HALF - 160 - button_prev->pos.w;
	button_prev->pos.y = VIEW_H_HALF - button_prev->pos.h;
	
	button_next = new WidgetButton(screen, font, inp, mods->locate("images/menus/buttons/right.png"));
	button_next->pos.x = VIEW_W_HALF + 160;
	button_next->pos.y = VIEW_H_HALF - button_next->pos.h;

	input_name = new WidgetInput(screen, font, inp);
	input_name->setPosition(VIEW_W_HALF - input_name->pos.w/2, VIEW_H_HALF+184);
	
	loadGraphics();
	loadOptions("hero_options.txt");
	loadPortrait(portrait[0]);
}	

void GameStateNew::loadGraphics() {
	portrait_border = NULL;
	
	portrait_border = IMG_Load(mods->locate("images/menus/portrait_border.png").c_str());
	if(!portrait_border) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	SDL_SetColorKey( portrait_border, SDL_SRCCOLORKEY, SDL_MapRGB(portrait_border->format, 255, 0, 255) ); 
	
	// optimize
	SDL_Surface *cleanup = portrait_border;
	portrait_border = SDL_DisplayFormatAlpha(portrait_border);
	SDL_FreeSurface(cleanup);
}

void GameStateNew::loadPortrait(string portrait_filename) {
	SDL_FreeSurface(portrait_image);
	portrait_image = NULL;
	
	portrait_image = IMG_Load(mods->locate("images/portraits/" + portrait_filename + ".png").c_str());
	if (!portrait_image) return;
	
	// optimize
	SDL_Surface *cleanup = portrait_image;
	portrait_image = SDL_DisplayFormatAlpha(portrait_image);
	SDL_FreeSurface(cleanup);
}

/**
 * Load body type "base" and portrait/head "portrait" options from a config file
 *
 * @param filename File containing entries for option=base,look
 */
void GameStateNew::loadOptions(string filename) {
	FileParser fin;
	if (!fin.open(mods->locate("engine/" + filename))) return;
	
	while (fin.next()) {
	
		// if at the max allowed base+look options, skip the rest of the file
		// TODO: remove static array size limit
		if (option_count == PLAYER_OPTION_MAX-1) break;

		if (fin.key == "option") {
			base[option_count] = fin.nextValue();
			head[option_count] = fin.nextValue();
			portrait[option_count] = fin.nextValue();
			option_count++;
		}
	}
	fin.close();
}

void GameStateNew::logic() {

	// require character name
	if (input_name->getText() == "") {
		button_create->enabled = false;
	}
	else {
		button_create->enabled = true;
	}

	if (button_exit->checkClick()) {
		requestedGameState = new GameStateLoad(screen, inp, font);
	}
	
	if (button_create->checkClick()) {
		// start the new game
		GameStatePlay* play = new GameStatePlay(screen, inp, font);
		play->pc->stats.base = base[current_option];
		play->pc->stats.head = head[current_option];
		play->pc->stats.portrait = portrait[current_option];
		play->pc->stats.name = input_name->getText();
		play->game_slot = game_slot;
		play->resetGame();
		requestedGameState = play;
	}
	
	// scroll through portrait options	
	if (button_next->checkClick()) {
		current_option++;
		if (current_option == option_count) current_option = 0;
		loadPortrait(portrait[current_option]);
	}
	else if (button_prev->checkClick()) {
		current_option--;
		if (current_option == -1) current_option = option_count-1;
		loadPortrait(portrait[current_option]);
	}

	input_name->logic();
	
}

void GameStateNew::render() {

	// display buttons
	button_exit->render();
	button_create->render();
	button_prev->render();
	button_next->render();
	input_name->render();
	
	// display portrait option
	SDL_Rect src;
	SDL_Rect dest;
	
	src.w = src.h = dest.w = dest.h = 320;
	src.x = src.y = 0;
	dest.x = VIEW_W_HALF - 160;
	dest.y = VIEW_H_HALF - 160;

	if (portrait != NULL) {
		SDL_BlitSurface(portrait_image, &src, screen, &dest);		
	}
	if (portrait_border != NULL) {
		SDL_BlitSurface(portrait_border, &src, screen, &dest);
	}
	
	// display labels
	font->render(msg->get("Choose a Portrait"), VIEW_W_HALF, VIEW_H_HALF-176, JUSTIFY_CENTER, screen, FONT_GREY);
	font->render(msg->get("Choose a Name"), VIEW_W_HALF, VIEW_H_HALF+168, JUSTIFY_CENTER, screen, FONT_GREY);
	
}

GameStateNew::~GameStateNew() {
	SDL_FreeSurface(portrait_image);
	SDL_FreeSurface(portrait_border);
	delete button_exit;
	delete button_create;
	delete button_next;
	delete button_prev;
	delete input_name;
}
