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
 * GameStateOptions
 *
 * Handle additional player options when starting a new game
 * (e.g. difficulty, permadeath)
 */

#include "Avatar.h"
#include "FileParser.h"
#include "GameStateConfig.h"
#include "GameStateOptions.h"
#include "GameStateNew.h"
#include "GameStateLoad.h"
#include "GameStatePlay.h"
#include "Settings.h"
#include "SharedResources.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetLabel.h"
#include "WidgetComboBox.h"

using namespace std;


GameStateOptions::GameStateOptions() : GameState() {
	game_slot = 0;

	// set up buttons
	button_start = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_start->label = msg->get("Start Game");
	button_start->pos.x = VIEW_W_HALF;
	button_start->pos.y = VIEW_H - button_start->pos.h;
	button_start->refresh();
	
	button_exit = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_exit->label = msg->get("Cancel");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	button_exit->refresh();

	button_difficulty = new WidgetComboBox(3,mods->locate("images/menus/buttons/combobox_default.png"));
	button_difficulty->pos.x = VIEW_W_HALF - button_difficulty->pos.w/2;
	button_difficulty->pos.y = VIEW_H_HALF;
//	button_difficulty->set(0,"EASY");
//	button_difficulty->set(1,"NORMAL");
//	button_difficulty->set(2,"HARD");
	setupDifficultyList();
	button_difficulty->selected = 1;
	button_difficulty->refresh();

	button_permadeath = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	button_permadeath->pos.x = button_difficulty->pos.x;
	button_permadeath->pos.y = button_difficulty->pos.y + button_difficulty->pos.h + 5;
	
	// set up labels
	label_difficulty = new WidgetLabel();
	label_difficulty->set(button_difficulty->pos.x, button_difficulty->pos.y - button_difficulty->pos.h/2,
															JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Difficulty"), FONT_GREY);
	label_permadeath = new WidgetLabel();
	label_permadeath->set(button_permadeath->pos.x + button_permadeath->pos.w + 5, button_permadeath->pos.y + button_permadeath->pos.h/2,
															JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Permadeath?"), FONT_GREY);
}

void GameStateOptions::setupDifficultyList()
{
	FileParser infile;
	if (infile.open(mods->locate("engine/difficulty.txt").c_str()))	
	{
		unsigned int i=0;
		while (infile.next()) {
			   	button_difficulty->set(i,infile.key);
			   i += 1;
			}
	} 
	else fprintf(stderr, "Unable to open languages.txt!\n");
	infile.close();
}

void GameStateOptions::loadDifficultySettings() 
{
	FileParser infile;
	if (infile.open(mods->locate("engine/difficulty.txt"))) 
	{
		unsigned int i=0;
		while (infile.next()) {
			   if(button_difficulty->selected == i) {
				   difficulty = atof(infile.val.c_str());
			   }
			   i += 1;
			}
	} 
	else {
		difficulty = 1.0f;
		fprintf(stderr, "No difficulty config found! Setting difficulty to normal by default.\n");
	}

}

void GameStateOptions::logic() {
	if(button_difficulty->checkClick()) button_difficulty->render();
	button_permadeath->checkClick();

	if (button_exit->checkClick()) {
		delete requestedGameState;
		GameStateNew* newgame = new GameStateNew();
		newgame->game_slot = game_slot;
		newgame->name = name;
		newgame->current_option = current_option;
		requestedGameState = newgame;
	}

	if (button_start->checkClick()) {
		loadDifficultySettings();
		
		// start the new game
		GameStatePlay* play = new GameStatePlay();
		play->pc->stats.base = base;
		play->pc->stats.head = head;
		play->pc->stats.portrait = portrait;
		play->pc->stats.name = name;
		play->pc->stats.permadeath = button_permadeath->isChecked();
		play->game_slot = game_slot;
		play->resetGame();
		requestedGameState = play;
	}

}

void GameStateOptions::render() {
	// display labels
	label_permadeath->render();
	label_difficulty->render();	
	
	// display buttons
	button_exit->render();
	button_start->render();
	button_permadeath->render();
	button_difficulty->render();
}

GameStateOptions::~GameStateOptions() {

	delete button_exit;
	delete button_start;
	delete button_difficulty;
	delete label_difficulty;
	delete button_permadeath;
	delete label_permadeath;	
}
