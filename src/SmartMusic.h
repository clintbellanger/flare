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
 * class SmartMusic
 *
 * Wraps a Mix_Music, calling Mix_FreeMusic on it when it goes out of scope.
 */
 
#ifndef SMART_MUSIC_H
#define SMART_MUSIC_H

#include <SDL_mixer.h>

#include <string>

class SmartMusic {
private:
	Mix_Music* music_;
	SmartMusic(SmartMusic const& music);
	SmartMusic& operator=(SmartMusic const& music);

public:
	SmartMusic();
	explicit SmartMusic(Mix_Music* music);
	~SmartMusic();

	Mix_Music* release();
	// fails if Mix_Music is invalid.
	Mix_Music* get() const;
	bool is_null() const;
	void reset(Mix_Music* music = NULL);
	void reset_and_load(std::string const& name);

	// noop if music is null
	void play(int loops);
	void halt();

	operator bool() const;
	bool operator!() const;

	Mix_Music& operator*();
	Mix_Music const& operator*() const;
	Mix_Music* operator->();
	Mix_Music const* operator->() const;
};

#endif

