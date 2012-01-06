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
 * class SmartChunk
 *
 * Wraps a Mix_Chunk, calling Mix_FreeChunk on it when it goes out of scope.
 */
 
#ifndef SMART_CHUNK_H
#define SMART_CHUNK_H

#include <string>

class Mix_Chunk;

class SmartChunk {
private:
	Mix_Chunk* chunk_;
	SmartChunk(SmartChunk const& chunk);
	SmartChunk& operator=(SmartChunk const& chunk);

public:
	SmartChunk();
	explicit SmartChunk(Mix_Chunk* chunk);
	~SmartChunk();

	Mix_Chunk* release();
	// fails if Mix_Chunk is invalid.
	Mix_Chunk* get() const;
	bool is_null() const;
	void reset(Mix_Chunk* chunk = NULL);
	void reset_and_load(std::string const& name);

	// noop if chunk is null
	void play_channel(int i, int j);

	operator bool() const;
	bool operator!() const;

	Mix_Chunk& operator*();
	Mix_Chunk const& operator*() const;
	Mix_Chunk* operator->();
	Mix_Chunk const* operator->() const;
};

#endif
