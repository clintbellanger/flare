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
 * class WidgetTooltip
 */

#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include "Array.h"
#include "Assert.h"
#include "FontEngine.h"
#include "Settings.h"
#include "SmartSurface.h"
#include "Utils.h"

#include <SDL.h>

const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

const int TOOLTIP_MAX_LINES = 16;

// Warning!  Copying does not copy over all members!
// If you want to really copy everything over completely, use .copy().
// If you want to steal the members (leave them empty in the original),
// use .steal().
struct TooltipData {
	int num_lines;
	Array<std::string, TOOLTIP_MAX_LINES> lines;
	Array<int, TOOLTIP_MAX_LINES> colors;
	SmartSurface tip_buffer;
	
	TooltipData() : num_lines(0), colors(0) {
	}

	TooltipData(TooltipData const& tip) : num_lines(tip.num_lines),
		lines(tip.lines), colors(tip.colors) {
		FlareSoftAssert(!tip.tip_buffer, "A TooltipData with a non-null buffer has been copied over: possible trouble ahead.");
	}

	TooltipData& operator=(TooltipData const& tip) {
		if (this == &tip)
			return *this;
		FlareSoftAssert(!tip.tip_buffer, "A TooltipData with a non-null buffer has been copied over: possible trouble ahead.");
		num_lines = tip.num_lines;
		lines = tip.lines;
		colors = tip.colors;
		return *this;
	}

	void clear() {
		num_lines = 0;
		lines.reset();
		colors.reset();
		tip_buffer.reset();
	}
	
	void copy(TooltipData const& tip) {
		FlareAssert(!"Not implemented -- don't know enough SDL.");
		// TODO: This should eventually be implemented.
	}

	void steal(TooltipData& tip) {
		if (this == &tip)
			return;
		num_lines = tip.num_lines;
		lines = tip.lines;
		colors = tip.colors;
		tip_buffer.steal(tip.tip_buffer);
	}
};

class WidgetTooltip {
private:
	int offset;
	int width;
	int margin;
public:
	WidgetTooltip();
	Point calcPosition(int style, Point pos, Point size);
	void render(TooltipData &tip, Point pos, int style);
	void clear(TooltipData &tip);
	void createBuffer(TooltipData &tip);
};

#endif
