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

#include "cleanup.h"
#include "FontEngine.h"
#include "GameSwitcher.h"
#include "InputState.h"
#include "MessageEngine.h"
#include "ModManager.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

void cleanup() {
	delete gswitch;
	
	delete font;
	delete inp;
	delete msg;
	delete mods;
	SDL_FreeSurface(screen);
	
	Mix_CloseAudio();
	SDL_Quit();
}

