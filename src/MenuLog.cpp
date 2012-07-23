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
 * class MenuLog
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuLog.h"
#include "ModManager.h"
#include "Settings.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetScrollBox.h"
#include "WidgetTabControl.h"

using namespace std;


MenuLog::MenuLog() {

	visible = false;
	tab_content_indent = 4;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/log.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "title") {
				title_pos.x = eatFirstInt(infile.val,',');
				title_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "tab_area") {
				tab_area.x = eatFirstInt(infile.val,',');
				tab_area.y = eatFirstInt(infile.val,',');
				tab_area.w = eatFirstInt(infile.val,',');
				tab_area.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "tab_content_offset") {
				tab_content_y = eatFirstInt(infile.val,',');
			}
		}
	} else fprintf(stderr, "Unable to open log.txt!\n");

	// Store the amount of displayed log messages on each log, and the maximum.
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		msg_buffer[i] = new WidgetScrollBox(tab_area.w,tab_area.h);
	}

	// Initialize the tab control.
	tabControl = new WidgetTabControl(LOG_TYPE_COUNT);

	// Define the header.
	tabControl->setTabTitle(LOG_TYPE_MESSAGES, msg->get("Messages"));
	tabControl->setTabTitle(LOG_TYPE_QUESTS, msg->get("Quests"));
	tabControl->setTabTitle(LOG_TYPE_STATISTICS, msg->get("Statistics"));

	paragraph_spacing = font->getLineHeight()/2;

	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

}

void MenuLog::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/log.png").c_str());

	if(!background) {
		fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

}

void MenuLog::update() {
	tabControl->setMainArea(window_area.x + tab_area.x, window_area.y + tab_area.y, tab_area.w, tab_area.h);
	tabControl->updateHeader();
	closeButton->pos.x = window_area.x + close_pos.x;
	closeButton->pos.y = window_area.y + close_pos.y;

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		msg_buffer[i]->pos.x = window_area.x+tab_area.x;
		msg_buffer[i]->pos.y = window_area.y+tab_area.y+tab_content_y;
	}
}

/**
 * Perform one frame of logic.
 */
void MenuLog::logic() {
	if(!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		msg_buffer[i]->refresh();
		msg_buffer[i]->logic();
	}
}

/**
 * Run the logic for the tabs control.
 */
void MenuLog::tabsLogic()
{
	tabControl->logic();
}


/**
 * Render graphics for this frame when the menu is open
 */
void MenuLog::render() {

	if (!visible) return;

	SDL_Rect src,dest;

	// Background.
	dest = window_area;
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &window_area);

	// Close button.
	closeButton->render();

	// Text overlay.
	WidgetLabel label;
	label.set(window_area.x+title_pos.x, window_area.y+title_pos.y, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Log"), FONT_WHITE);
	label.render();

	// Tab control.
	tabControl->render();

	// Display latest log messages for the active tab.

	int display_number = 0;
	int total_size = tab_content_indent;
	int active_log = tabControl->getActiveTab();
	SDL_Rect contentArea = tabControl->getContentArea();

	if (msg_buffer[active_log]->update) {
		for (unsigned int i=log_msg[active_log].size(); i>0; i--) {
			int widthLimit = tabControl->getContentArea().w;
			Point size = font->calc_size(log_msg[active_log][i-1], widthLimit);
			font->renderShadowed(log_msg[active_log][i-1], tab_content_indent, total_size, JUSTIFY_LEFT, msg_buffer[active_log]->contents, widthLimit, FONT_WHITE);
			total_size+=size.y+paragraph_spacing;
		}
	}

	msg_buffer[active_log]->render();
}

void MenuLog::refresh(int log_type) {
	int y = tab_content_indent;

	for (unsigned int i=0; i<log_msg[log_type].size(); i++) {
		int widthLimit = tabControl->getContentArea().w;
		Point size = font->calc_size(log_msg[log_type][i], widthLimit);
		y+=size.y+paragraph_spacing;
	}
	y+=tab_content_indent;

	msg_buffer[log_type]->resize(y);
}

/**
 * Add a new message to the log.
 */
void MenuLog::add(const string& s, int log_type) {

	// Add the new message.
	log_msg[log_type].push_back(s);
	msg_buffer[log_type]->update = true;
	refresh(log_type);

	log_count[log_type]++;
}

/**
 * Remove log message with the given identifier.
 */
void MenuLog::remove(int msg_index, int log_type) {

	log_msg[log_type][msg_index].erase();
	msg_buffer[log_type]->update = true;
	refresh(log_type);

	log_count[log_type]--;
}

void MenuLog::clear(int log_type) {
	log_count[log_type] = 0;
	log_msg[log_type].clear();
	msg_buffer[log_type]->update = true;
	refresh(log_type);
}

void MenuLog::clear() {
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		clear(i);
	}
}

MenuLog::~MenuLog() {

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		delete msg_buffer[i];
	}

	SDL_FreeSurface(background);
	delete closeButton;
	delete tabControl;
}
