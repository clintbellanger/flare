/*
Copyright 2011 kitano

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
 * class MenuExit
 */

#ifndef MENU_EXIT_h
#define MENU_EXIT_H

#include "Menu.h"
#include "SDL_image.h"
#include "WidgetButton.h"
#include "MessageEngine.h"

class MenuExit : public Menu {
protected:
	void loadGraphics();

	WidgetButton *buttonExit;
	WidgetButton *buttonClose;

	bool exitClicked;

public:
	MenuExit(SDL_Surface*, InputState*, FontEngine*);
	~MenuExit();

	void logic();
	virtual void render();

	bool isExitRequested() { return exitClicked; }
};

#endif
