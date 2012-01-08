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

#include "SmartSurface.h"
#include "Assert.h"
#include "ModManager.h"
#include "SharedResources.h"

#include <SDL_image.h>


SmartSurface::SmartSurface() : surface_(NULL) {}

SmartSurface::SmartSurface(SDL_Surface* surface) : surface_(surface) {}

SmartSurface::~SmartSurface() {
	reset();
}

SDL_Surface* SmartSurface::release() {
	SDL_Surface* retval = surface_;
	surface_ = NULL;
	return retval;
}

SDL_Surface* SmartSurface::get() const {
	FlareAssert(surface_ && "Invalid surface requested");
	return surface_;
}

bool SmartSurface::is_null() const {
	return surface_ == NULL;
}

void SmartSurface::reset(SDL_Surface* surface) {
	if (surface_)
		SDL_FreeSurface(surface_);
	surface_ = surface;
}

void SmartSurface::reset_and_load(std::string const& name) {
	reset(IMG_Load(mods->locate(name).c_str()));
	FlareSoftAssert(!is_null(), ("Couldn't load image: " + name).c_str());
}

void SmartSurface::steal(SmartSurface& surface) {
	reset(surface.release());
}

void SmartSurface::display_format_alpha() {
	FlareSoftAssert(surface_, "Operation performed on invalid surface.");
	// Optimize?
	if (surface_)
		reset(SDL_DisplayFormatAlpha(surface_));
}

void SmartSurface::set_color_key(Uint32 flag, Uint32 key) {
	FlareSoftAssert(surface_, "Operation performed on invalid surface.");
	if (surface_)
		SDL_SetColorKey(surface_, flag, key); 
}

Uint32 SmartSurface::map_rgb(Uint8 r, Uint8 g, Uint8 b) {
	FlareSoftAssert(surface_, "Operation performed on invalid surface.");
	if (surface_)
		return SDL_MapRGB(surface_->format, r, g, b);
	else
		return 0; // Currently acts as error code.
}

void SmartSurface::set_alpha(Uint32 flags, Uint8 alpha) {
	FlareSoftAssert(surface_, "Operation performed on invalid surface.");
	if (surface_)
		SDL_SetAlpha(surface_, flags, alpha);
}

void SmartSurface::fill_rect(SDL_Rect* dstrect, Uint32 color) {
	FlareSoftAssert(surface_, "Operation performed on invalid surface.");
	if (surface_)
		SDL_FillRect(surface_, dstrect, color);
}

SmartSurface::operator bool() const {
	return surface_ != NULL;
}

bool SmartSurface::operator!() const{
	return !surface_;
}

SDL_Surface& SmartSurface::operator*() {
	FlareAssert(surface_ && "Invalid surface dereferenced.");
	return *surface_;
}

SDL_Surface const& SmartSurface::operator*() const {
	FlareAssert(surface_ && "Invalid surface dereferenced.");
	return *surface_;
}

SDL_Surface* SmartSurface::operator->() {
	FlareAssert(surface_ && "Invalid surface dereferenced.");
	return surface_;
}

SDL_Surface const* SmartSurface::operator->() const {
	FlareAssert(surface_ && "Invalid surface dereferenced.");
	return surface_;
}

