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

#include "SmartMusic.h"
#include "Assert.h"
#include "ModManager.h"
#include "SharedResources.h"

SmartMusic::SmartMusic() : music_(NULL) {}

SmartMusic::SmartMusic(Mix_Music* music) : music_(music) {}

SmartMusic::~SmartMusic() {
	reset();
}

Mix_Music* SmartMusic::release() {
	Mix_Music* retval = music_;
	music_ = NULL;
	return retval;
}

Mix_Music* SmartMusic::get() const {
	FlareAssert(music_ && "Invalid music requested");
	return music_;
}

bool SmartMusic::is_null() const {
	return music_ == NULL;
}

void SmartMusic::reset(Mix_Music* music) {
	halt();
	if (music_)
		Mix_FreeMusic(music_);
	music_ = music;
}

void SmartMusic::reset_and_load(std::string const& name) {
	reset(Mix_LoadMUS((mods->locate(name)).c_str()));
	FlareSoftAssert(!is_null(), ("Couldn't load music: " + name).c_str());
}

void SmartMusic::play(int loops) {
	if (!is_null())
		Mix_PlayMusic(music_, loops);
}

void SmartMusic::halt() {
	if (!is_null())
		Mix_HaltMusic();
}

SmartMusic::operator bool() const {
	return music_ != NULL;
}

bool SmartMusic::operator!() const{
	return !music_;
}

Mix_Music& SmartMusic::operator*() {
	FlareAssert(music_ && "Invalid music dereferenced.");
	return *music_;
}

Mix_Music const& SmartMusic::operator*() const {
	FlareAssert(music_ && "Invalid music dereferenced.");
	return *music_;
}

Mix_Music* SmartMusic::operator->() {
	FlareAssert(music_ && "Invalid music dereferenced.");
	return music_;
}

Mix_Music const* SmartMusic::operator->() const {
	FlareAssert(music_ && "Invalid music dereferenced.");
	return music_;
}



