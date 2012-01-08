/*
Copyright 2011 Clint Bellanger and morris989

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
 * class MenuTalker
 */

#ifndef MENU_TALKER_H
#define MENU_TALKER_H

#include "ScopedPtr.h"
#include "SmartSurface.h"

#include <string>

class SDL_Surface;

class CampaignManager;
class NPC;
class WidgetButton;

class MenuTalker {
private:
	CampaignManager *camp;

	void loadGraphics();
	SmartSurface background;
	SmartSurface portrait;
	SmartSurface msg_buffer;
	std::string hero_name;

	int dialog_node;

public:
	MenuTalker(CampaignManager *camp);

	NPC *npc;
	
	void chooseDialogNode();
	void logic();
	void render();
	void setHero(const std::string& name, const std::string& portrait_filename);
	void createBuffer();
	
	bool visible;
	int event_cursor;
	bool accept_lock;

	ScopedPtr<WidgetButton> advanceButton;
	ScopedPtr<WidgetButton> closeButton;
	
};

#endif
