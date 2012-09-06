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

#include <sstream>
#include <SDL.h>

#include "GameTime.h"
#include "Settings.h"
#include "UtilsParsing.h"

const int TIME_UPDATE_INTERVAL = 100;

GameTime::GameTime() {
        ticks = 0;
}

GameTime::~GameTime() {
}

void GameTime::update() {
        unsigned int cticks = SDL_GetTicks();
		if ((cticks - ticks) > TIME_UPDATE_INTERVAL) 
		{
		        real.addMs((cticks-ticks));
				ingame.addMs((cticks-ticks) * TIME_FACTOR);
				ticks = cticks;
		}
}

void GameTime::reset() {
        ticks = SDL_GetTicks();
		ingame.reset();
		real.reset();
}

Time::Time() {
        reset();
}

Time::Time(std::string canonical) {
        fromCanonicalString(canonical);
}



Time::~Time() {
}

/** \brief Initialize time from canonical string.
    \note Canonincal time format is days:hours:minutes:seconds:ms
*/
void Time::fromCanonicalString(std::string canonical) {
        int i = 0;
		std::string s;
		std::stringstream ss(canonical);
		unsigned int v[5] = {0,0,0,0,0};
		
		while (getline(ss, s, ':')) {
		  v[i] = toInt(s);
		  i++;
		}

        fromVector(v);
}

/** \brief Get canonical string of time.
*/
std::string Time::toCanonicalString() {
        std::stringstream ss("");
		ss << days << ":" << hours << ":" << minutes << ":" << seconds << ":" << ms;
		return ss.str();
}

/** \brief Set time from vector of time components.
 */
void Time::fromVector(unsigned int *v) {
        days    = v[0];
		hours   = v[1];  
		minutes = v[2];
		seconds = v[3];
		ms      = v[4];
}

/** \brief Get display string representation of time object.
	\param[in] [current] True for current time and False for duration. 
*/
std::string Time::toString(bool current) {
        std::string tof;
		std::stringstream ss("");

		if (current) {
		        if (hours >= 5 && hours < 12)        tof = "Morning";
				else if (hours >= 12 && hours < 13)  tof = "Noon";
				else if (hours >= 13 && hours < 18)  tof = "Afternoon";
				else if (hours >= 18 && hours < 20)  tof = "Evening";
				else if (hours >= 20 && hours < 24)  tof = "Night";
				else if (hours >= 0 && hours < 1)    tof = "Midnight";
				else if (hours >= 1 && hours < 5)    tof = "Late night";
				ss << "Day " <<  days+1 << " - " << tof;
		}
		else
		        ss << days << " days, " << hours << " hours and " << minutes << " minutes.";

		return ss.str();
}

/** \brief Increase Time with milliseconds. 
*/
void Time::addMs(unsigned int millis) {
        ms += millis;
		unsigned int secs = (ms / 1000.0f);
		if (secs > 0) {
		        seconds += secs;
				ms -= secs*1000;
				recalculate();
		}
}

/** \brief Recalculate time. 
*/
void Time::recalculate() {
        if(seconds >= 60) {
		        minutes += 1 + (seconds % 60);
				seconds = 0;
		}

		if(minutes >= 60) {
		        hours += 1 + (minutes % 60);
				minutes = 0;
		}

		if(hours >= 24) {
		        days += 1 + (hours % 24);
				hours = 0;
		}
}

/** \brief Reset time. 
*/
void Time::reset() {
  days = hours = minutes = seconds = ms = 0;
}
