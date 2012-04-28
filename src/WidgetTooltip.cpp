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

#include "WidgetTooltip.h"
#include "SharedResources.h"
#include "Utils.h"

#include <sstream>

using namespace std;

WidgetTooltip::WidgetTooltip()
	: lines()
	, dirty(true)
	, surface(NULL) {
}

WidgetTooltip::~WidgetTooltip() {
	clear();
}

void WidgetTooltip::addLine(const string &text, int color) {
	lines.push_back(Line(text, color));
	dirty = true;
}

/**
 * Clear the given tooltip.
 * Free the buffered tip image
 */
void WidgetTooltip::clear() {
	lines.clear();
	if (surface) {
		SDL_FreeSurface(surface);
		surface = NULL;
	}
}

/**
 * Render tooltip surface only when the contents of the tooltip has changed of
 * the surface has not yet been rendered.
 */
void WidgetTooltip::initSurface() const {

	if (surface) {
		SDL_FreeSurface(surface);
	}

	// concat multi-line tooltip, used in determining total display size
	stringstream fulltext;
	bool firstLine = true;
	for (Lines::const_iterator i = lines.begin(); i != lines.end(); ++i) {
		if (!firstLine) {
			fulltext << "\n";
		}
		fulltext << i->text;
		firstLine = false;
	}

	// calculate the full size to display a multi-line tooltip
	Point size = font->calc_size(fulltext.str(), width);

	surface = createSurface(size.x + margin * 2, size.y + margin * 2);

	// Currently tooltips are always opaque
	SDL_SetAlpha(surface, 0, 0);

	// style the tooltip background
	// currently this is plain black
	SDL_FillRect(surface, NULL, 0);

	int cursor_y = margin;

	for (Lines::const_iterator i = lines.begin(); i != lines.end(); ++i) {
		font->render(i->text, margin, cursor_y, JUSTIFY_LEFT, surface, size.x, i->color);
		cursor_y = font->cursor_y;
	}

	dirty = false;
}


/**
 * Knowing the total size of the text and the position of origin,
 * calculate the starting position of the background and text
 */
Point WidgetTooltip::calcPosition(int style, const Point &pos) const {

	Point tip_pos;

	// (lazy/re)-init surface if needed
	if (!surface || dirty) {
		initSurface();
	}

	// TopLabel style is fixed and centered over the origin
	if (style == STYLE_TOPLABEL) {
		tip_pos.x = pos.x - surface->w / 2;
		tip_pos.y = pos.y - offset;
	}
	// Float style changes position based on the screen quadrant of the origin
	// (usually used for tooltips which are long and we don't want them to overflow
	//  off the end of the screen)
	else if (style == STYLE_FLOAT) {
		// upper left
		if (pos.x < VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			tip_pos.x = pos.x + offset;
			tip_pos.y = pos.y + offset;
		}
		// upper right
		else if (pos.x >= VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			tip_pos.x = pos.x - offset - surface->w;
			tip_pos.y = pos.y + offset;
		}
		// lower left
		else if (pos.x < VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			tip_pos.x = pos.x + offset;
			tip_pos.y = pos.y - offset - surface->h;
		}
		// lower right
		else if (pos.x >= VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			tip_pos.x = pos.x - offset - surface->w;
			tip_pos.y = pos.y - offset - surface->h;
		}
	}

	return tip_pos;
}

/**
 * Tooltip position depends on the screen quadrant of the source.
 * Draw the buffered tooltip if it exists, else render the tooltip and buffer it
 */
void WidgetTooltip::render(const Point &pos, int style) const {

	// (lazy/re)-init surface if needed
	if (!surface || dirty) {
		initSurface();
	}

	Point size;
	size.x = surface->w;
	size.y = surface->h;

	Point tip_pos = calcPosition(style, pos);

	SDL_Rect dest;
	dest.x = tip_pos.x;
	dest.y = tip_pos.y;

	SDL_BlitSurface(surface, NULL, screen, &dest);
}

