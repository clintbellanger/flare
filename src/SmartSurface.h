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
 * class SmartSurface
 *
 * Wraps an SDL_Surface, calling SDL_Surface on it when it goes out of scope
 */
 
#ifndef SMART_SURFACE_H
#define SMART_SURFACE_H

#include <SDL.h>

#include <string>

class SDL_Surface;

class SmartSurface {
private:
	SDL_Surface* surface_;
	SmartSurface(SmartSurface const& surface);
	SmartSurface& operator=(SmartSurface const& surface);

public:
	SmartSurface();
	explicit SmartSurface(SDL_Surface* surface);
	~SmartSurface();

	SDL_Surface* release();
	// fails if SDL_Surface is invalid.
	SDL_Surface* get() const;
	bool is_null() const;
	void reset(SDL_Surface* surface = NULL);
	void reset_and_load(std::string const& name);
	void steal(SmartSurface& surface);
	void display_format_alpha();
	void set_color_key(Uint32 flag, Uint32 key);
	Uint32 map_rgb(Uint8 r, Uint8 g, Uint8 b);
	void set_alpha(Uint32 flags, Uint8 alpha);
	void fill_rect(SDL_Rect* dstrect, Uint32 color);
	

	operator bool() const;
	bool operator!() const;

	SDL_Surface& operator*();
	SDL_Surface const& operator*() const;
	SDL_Surface* operator->();
	SDL_Surface const* operator->() const;
};

#endif
