/*
Copyright © 2011-2012 Clint Bellanger and Thane Brimhall
Copyright © 2012 Henrik Andersson

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

#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H

#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <fstream>
#include <string>
#include <map>
#include <list>

const int JUSTIFY_LEFT = 0;
const int JUSTIFY_RIGHT = 1;
const int JUSTIFY_CENTER = 2;

const SDL_Color FONT_WHITE = {255,255,255,0};
const SDL_Color FONT_BLACK = {0,0,0,0};

class Text_Surface {
public:
	Text_Surface() {
		surface = NULL;
	}
	~Text_Surface() {
		if (surface) SDL_FreeSurface(surface);
	}

	std::string text;
	bool blended;
	SDL_Color color;
	SDL_Surface *surface;
};

/**
 * class FontEngine
 * 
 * Handles rendering a bitmap font.
 */

class FontEngine {
private:
	int font_pt;
	int font_height;
	int line_height;
	SDL_Rect src;
	SDL_Rect dest;
	TTF_Font *ttfont;
	bool render_blended;
	std::map<std::string,SDL_Color> color_map;
	std::list<Text_Surface> cache;

	const Text_Surface *cache_lookup(std::string text, SDL_Color color, bool blended);
	SDL_Surface *get_surface(std::string text, SDL_Color color, bool blended);
public:
	FontEngine();
	~FontEngine();

	int getLineHeight() { return line_height; }
	int getFontHeight() { return font_height; }
	
	SDL_Color getColor(std::string _color);
	int calc_width(const std::string& text);
	Point calc_size(const std::string& text_with_newlines, int width);

	void render(const std::string& text, int x, int y, int justify, SDL_Surface *target, SDL_Color color);
	void render(const std::string& text, int x, int y, int justify, SDL_Surface *target, int width, SDL_Color color);
	void renderShadowed(const std::string& text, int x, int y, int justify, SDL_Surface *target, SDL_Color color);
	void renderShadowed(const std::string& text, int x, int y, int justify, SDL_Surface *target, int width, SDL_Color color);

	int cursor_y;
};

#endif
