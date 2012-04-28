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
 * class WidgetTooltip
 */

#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "FontEngine.h"
#include "Utils.h"
#include "Settings.h"

using std::vector;

const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

class WidgetTooltip : private Uncopyable {
public:
	struct Line {
		string text;
		int color;
		Line(const string &_text, int _color) : text(_text), color(_color) {}
	};
	typedef vector<Line> Lines;


private:
	Lines lines;
	mutable bool dirty;
	mutable SDL_Surface *surface;

	// TODO: Put these values in an engine config file
	static const int offset	= 12;	/**< distance between cursor and tooltip */
	static const int width	= 160;	/**< max width of tooltips (wrap text) */
	static const int margin	= 4;	/**< outer margin between tooltip text and the edge of the tooltip background */

public:
	WidgetTooltip();
	~WidgetTooltip();

	const Lines &getLines() const	{return const_cast<const Lines&>(lines);}
	bool isEmpty() const			{return lines.empty();}
	void clear();
	void addLine(const string &text, int color = FONT_WHITE);
	Point calcPosition(int style, const Point &pos) const;
	void render(const Point &pos, int style) const;


private:
	void initSurface() const;
};

#endif
