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

#ifndef COMBAT_TEXT_H
#define COMBAT_TEXT_H

#include "WidgetLabel.h"
#include "Settings.h"
#include <list>
#include <string>

#define DISPLAY_DAMAGE 0
#define DISPLAY_CRIT 1
#define DISPLAY_HEAL 2
#define DISPLAY_MISS 3
#define DISPLAY_SHIELD 4

class Combat_Text_Item : private Uncopyable {
	friend class CombatText;

private:
	WidgetLabel label;
    int lifespan;
    Point pos;
    string text;
    int displaytype;

public:
	Combat_Text_Item(int lifespan, const Point &pos, const string &text, int displaytype);
};

class CombatText : private Uncopyable {
public:
	typedef std::list<Combat_Text_Item *> Items;

    static CombatText* Instance();
    void render();
    void addMessage(const string &message, const Point &location, int displaytype);
    void addMessage(int num, const Point &location, int displaytype);
    void setCam(const Point &location);

private:
    Point cam;
    Items combat_text;
    CombatText(){};

    static CombatText* m_pInstance;

};

#endif
