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

#include "SmartChunk.h"
#include "Assert.h"
#include "ModManager.h"
#include "SharedResources.h"

#include <SDL_mixer.h>

SmartChunk::SmartChunk() : chunk_(NULL) {}

SmartChunk::SmartChunk(Mix_Chunk* chunk) : chunk_(chunk) {}

SmartChunk::~SmartChunk() {
	reset();
}

Mix_Chunk* SmartChunk::release() {
	Mix_Chunk* retval = chunk_;
	chunk_ = NULL;
	return retval;
}

Mix_Chunk* SmartChunk::get() const {
	FlareAssert(chunk_ && "Invalid chunk requested");
	return chunk_;
}

bool SmartChunk::is_null() const {
	return chunk_ == NULL;
}

void SmartChunk::reset(Mix_Chunk* chunk) {
	if (chunk_)
		Mix_FreeChunk(chunk_);
	chunk_ = chunk;
}

void SmartChunk::reset_and_load(std::string const& name) {
	reset(Mix_LoadWAV(mods->locate(name).c_str()));
	FlareSoftAssert(!is_null(), ("Couldn't load chunk: " + name).c_str());
}

void SmartChunk::play_channel(int channel, int loops) {
	if (!is_null())
		Mix_PlayChannel(channel, chunk_, loops);
}

SmartChunk::operator bool() const {
	return chunk_ != NULL;
}

bool SmartChunk::operator!() const{
	return !chunk_;
}

Mix_Chunk& SmartChunk::operator*() {
	FlareAssert(chunk_ && "Invalid chunk dereferenced.");
	return *chunk_;
}

Mix_Chunk const& SmartChunk::operator*() const {
	FlareAssert(chunk_ && "Invalid chunk dereferenced.");
	return *chunk_;
}

Mix_Chunk* SmartChunk::operator->() {
	FlareAssert(chunk_ && "Invalid chunk dereferenced.");
	return chunk_;
}

Mix_Chunk const* SmartChunk::operator->() const {
	FlareAssert(chunk_ && "Invalid chunk dereferenced.");
	return chunk_;
}


