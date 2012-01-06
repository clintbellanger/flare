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

#include "Assert.h"
#include "FontEngine.h"
#include "InputState.h"
#include "MessageEngine.h"
#include "ModManager.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_mixer.h>

#include <iostream>

void AssertionError(char const* exp, char const* file, char const* func, int line) {
	std::cerr << "Assertion failed!\n\n";
	std::cerr << file << ":" << line << ": " << func << ": " << exp << "\n\n";

	std::cerr << "A fatal error has occured.  Please file a bug report at "
		"https://github.com/clintbellanger/flare .\n";
	std::cerr << "Trying to clean up..." << std::endl;
	
	// TODO: Merge this with cleanup() from main.cpp
	// TODO: Find a way to delete gswitch
	delete font;
	delete inp;
	delete msg;
	delete mods;
	std::cerr << "Deleted shared resources." << std::endl;

	SDL_FreeSurface(screen);
	std::cerr << "Freed screen." << std::endl;
	
	Mix_CloseAudio();
	std::cerr << "Audio closed." << std::endl;
	
	SDL_Quit();
	std::cerr << "SDL closed." << std::endl;
	
	abort();
}

void SoftAssertionError(char const* exp, char const* file, char const* func, char const* err_msg, int line) {
	std::cerr << "Warning: ";
	std::cerr << file << ":" << line << ": " << func << ": `" << exp << "' failed: " << err_msg << std::endl;
}
