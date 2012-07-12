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

#ifndef GAMESTATEOPTIONS_H
#define GAMESTATEOPTIONS_H

#include "GameState.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <sstream>

class WidgetButton;
class WidgetCheckBox;
class WidgetLabel;
class WidgetComboBox;



class GameStateOptions : public GameState {
private:

	WidgetButton *button_exit;
	WidgetButton *button_start;
	WidgetCheckBox *button_permadeath;
	WidgetLabel *label_permadeath;
	WidgetComboBox *button_difficulty;
	WidgetLabel *label_difficulty;
	
	
public:
	GameStateOptions();
	~GameStateOptions();
	void logic();
	void render();
	void setupDifficultyList();
	void loadDifficultySettings();
	
	int game_slot;
	std::string base;
	std::string head;
	std::string portrait;
	std::string name;
	int current_option;	
	float difficulty;
};

#endif
