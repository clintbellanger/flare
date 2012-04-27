/*
Copyright © 2011-2012 Thane Brimhall

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
 * class CombatText
 *
 * The CombatText class displays floating damage numbers and miss messages
 * above the targets.
 * 
 */

#include "CombatText.h"
#include <iostream>
#include <sstream>
#include <cassert>

Combat_Text_Item::Combat_Text_Item(int _lifespan, const Point &_pos, const string &_text, int _displaytype)
	: label()
	, lifespan(_lifespan)
	, pos(_pos)
	, text(_text)
	, displaytype(_displaytype) {
}

// Global static pointer used to ensure a single instance of the class.
CombatText* CombatText::m_pInstance = NULL;  
  
CombatText* CombatText::Instance() {
   if (!m_pInstance)
      m_pInstance = new CombatText;

   return m_pInstance;
}

void CombatText::setCam(const Point &location) {
    cam = location;
}

void CombatText::addMessage(const string &message, const Point &location, int displaytype) {
    if (COMBAT_TEXT) {
	    Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
        combat_text.push_back(new Combat_Text_Item(30, p, message, displaytype));
    }
}

void CombatText::addMessage(int num, const Point &location, int displaytype) {
    if (COMBAT_TEXT) {
	    Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
        std::stringstream ss;
        ss << num;
        combat_text.push_back(new Combat_Text_Item(30, p, ss.str(), displaytype));
    }
}

void CombatText::render() {
	for(Items::iterator it = combat_text.begin(); it != combat_text.end(); ++it) {
		Combat_Text_Item &cti = **it;

		/* FIXME: display lifetime & scrolling up screen dependent upon frame rate */
		--cti.lifespan;
		--cti.pos.y;
        if (cti.lifespan > 0) {
			switch (cti.displaytype) {
			case DISPLAY_DAMAGE:
				cti.label.set(cti.pos.x, cti.pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, cti.text, FONT_WHITE);
				break;
			case DISPLAY_CRIT:
			case DISPLAY_MISS:
				cti.label.set(cti.pos.x, cti.pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, cti.text, FONT_RED);
				break;
			case DISPLAY_HEAL:
				cti.label.set(cti.pos.x, cti.pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, cti.text, FONT_GREEN);
				break;
			case DISPLAY_SHIELD:
				cti.label.set(cti.pos.x, cti.pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, cti.text, FONT_BLUE);
				break;
			default:
 				assert(0);
			}
		    cti.label.render();
		} else {
		}
    }
    // delete expired messages
    while (combat_text.size() > 0 && combat_text.front()->lifespan <= 0) {
		delete combat_text.front();
        combat_text.erase(combat_text.begin());
    }
}
