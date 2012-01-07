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

#ifndef GAMESTATENEW_H
#define GAMESTATENEW_H

#include "GameState.h"
#include "ScopedPtr.h"
#include "SmartSurface.h"
#include "StatBlock.h"

#include <SDL.h>

#include <string>
#include <sstream>

class ItemManager;
class MenuConfirm;
class WidgetInput;
class WidgetLabel;
class WidgetButton;


const int PLAYER_OPTION_MAX = 32;

class GameStateNew : public GameState {
private:

	void loadGraphics();
	void loadPortrait(const std::string& portrait_filename);
	void loadOptions(const std::string& option_filename);

	std::string base[PLAYER_OPTION_MAX];
	std::string head[PLAYER_OPTION_MAX];
	std::string portrait[PLAYER_OPTION_MAX];
	int option_count;
	int current_option;

	SmartSurface portrait_image;
	SmartSurface portrait_border;
	ScopedPtr<WidgetButton> button_exit;
	ScopedPtr<WidgetButton> button_create;
	ScopedPtr<WidgetButton> button_next;
	ScopedPtr<WidgetButton> button_prev;
	ScopedPtr<WidgetLabel> label_portrait;
	ScopedPtr<WidgetLabel> label_name;
	ScopedPtr<WidgetInput> input_name;

public:
	GameStateNew();
	void logic();
	void render();
	int game_slot;
	
};

#endif
