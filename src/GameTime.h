/*
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

/**
 * class WorlTime
 *
 * Handles logic of game engine World Time and a Time helper class
 *
 */

#include <string>

#ifndef WORLDTIME_H
#define WORLDTIME_H

class Time {
public:
        Time();
	Time(std::string canonical);
	virtual ~Time();
	void fromVector(unsigned int *v);
	void fromCanonicalString(std::string canonical);
	std::string toString(bool simplified=true);
	std::string toCanonicalString();
	void addMs(unsigned int ms);
	void reset();

protected:
        int days;
        int hours;
        int minutes;
	int seconds;
	int ms;

	void recalculate();
  
};

class GameTime {
public:
        GameTime();
        ~GameTime();
	
        void update();
	void reset();
	
	Time ingame;
	Time real;

protected:
	unsigned int ticks;
};

#endif
