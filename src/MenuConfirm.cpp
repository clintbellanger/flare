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

#include "MenuConfirm.h"
#include "FontEngine.h"
#include "ModManager.h"
#include "SharedResources.h"
#include "WidgetButton.h"
#include "WidgetLabel.h"

#include <SDL_image.h>

using namespace std;


MenuConfirm::MenuConfirm(const string& _buttonMsg, const string& _boxMsg) : Menu() {
	confirmClicked = false;

	// Text to display in confirmation box
	boxMsg = _boxMsg;

	window_area.w = 192;
	window_area.h = 64;
	window_area.x = (VIEW_W/2) - (window_area.w/2);
	window_area.y = (VIEW_H - window_area.h)/2;
	
	buttonConfirm.reset(new WidgetButton("images/menus/buttons/button_default.png"));
	buttonConfirm->label = _buttonMsg;
	buttonConfirm->pos.x = VIEW_W_HALF - buttonConfirm->pos.w/2;
	buttonConfirm->pos.y = VIEW_H/2;
	buttonConfirm->refresh();
	
	buttonClose.reset(new WidgetButton("images/menus/buttons/button_x.png"));
	buttonClose->pos.x = window_area.x + window_area.w;
	buttonClose->pos.y = window_area.y;

	label.set(window_area.x + window_area.w/2, window_area.y + 10, JUSTIFY_CENTER, VALIGN_TOP, boxMsg, FONT_WHITE);

	loadGraphics();
}

void MenuConfirm::loadGraphics() {
	background.reset_and_load("images/menus/confirm_bg.png");
	background.display_format_alpha();
}

void MenuConfirm::logic() {
	if (visible) {
	  if(buttonConfirm->checkClick())
	    confirmClicked = true;
	  if(buttonClose->checkClick())
	    visible = false;
	}
}

void MenuConfirm::render() {
	SDL_Rect src;

	// background
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background.get(), &src, screen, &window_area);

	label.render();

	buttonConfirm->render();
	buttonClose->render();
}

