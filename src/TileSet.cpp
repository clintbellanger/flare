/*
Copyright © 2011-2012 Clint Bellanger

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
 * class TileSet
 *
 * TileSet storage and file loading
 */

#include "TileSet.h"
#include "SharedResources.h"
#include "FileParser.h"
#include "UtilsParsing.h"
#include "Settings.h"

#include <cstdio>

using namespace std;

TileSet::TileSet() {
	sprites = NULL;
	reset();
}

void TileSet::reset() {

	SDL_FreeSurface(sprites);

	alpha_background = true;
	trans_r = 255;
	trans_g = 0;
	trans_b = 255;

	sprites = NULL;
	for (int i=0; i<TILE_SET_MAX_TILES; i++) {
		tiles[i].src.x = 0;
		tiles[i].src.y = 0;
		tiles[i].src.w = 0;
		tiles[i].src.h = 0;
		tiles[i].offset.x = 0;
		tiles[i].offset.y = 0;
	}
	
	for (int i = 0; i < TILE_SET_MAX_TILES; i++) {
		anim[i].current_frame = 1;
		anim[i].duration = 0;
		anim[i].frames = 1;
	}	
}

void TileSet::loadGraphics(const std::string& filename) {
	if (sprites) SDL_FreeSurface(sprites);
	
	if (TEXTURE_QUALITY == false)
		sprites = IMG_Load((mods->locate("images/tilesets/noalpha/" + filename)).c_str());

	if (!sprites) {
		sprites = IMG_Load((mods->locate("images/tilesets/" + filename)).c_str());
		if (!sprites) {
			fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
			SDL_Quit();
		}
	} else {
		alpha_background = false;
	}
	
	// only set a color key if the tile set doesn't have an alpha channel
	// the color ke is specified in the tilesetdef file like this:
	// transparency=r,g,b
	if (!alpha_background) {
		SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, SDL_MapRGB(sprites->format, trans_r, trans_g, trans_b) );
	}
	
	// optimize
	SDL_Surface *cleanup = sprites;
	sprites = SDL_DisplayFormatAlpha(sprites);
	SDL_FreeSurface(cleanup);
}

void TileSet::load(const std::string& filename) {
	if (current_map == filename) return;
	
	reset();
	
	FileParser infile;
	unsigned short index;
	string img;

	if (infile.open(mods->locate("tilesetdefs/" + filename))) {
		while (infile.next()) {
			if (infile.key == "tile") {

				infile.val = infile.val + ',';
				index = eatFirstInt(infile.val, ',');
				
				if (index > 0 && index < TILE_SET_MAX_TILES) {
				
					tiles[index].src.x = eatFirstInt(infile.val, ',');
					tiles[index].src.y = eatFirstInt(infile.val, ',');
					tiles[index].src.w = eatFirstInt(infile.val, ',');
					tiles[index].src.h = eatFirstInt(infile.val, ',');
					tiles[index].offset.x = eatFirstInt(infile.val, ',');
					tiles[index].offset.y = eatFirstInt(infile.val, ',');
				}
				else {
					fprintf(stderr, "Warning: invalid tileset index in %s. Expected a value from 1 to %d\n", filename.c_str(), TILE_SET_MAX_TILES);
				}
				
			}
			else if (infile.key == "img") {
				img = infile.val;
			}
			else if (infile.key == "transparency") {
				alpha_background = false;
				
				infile.val = infile.val + ',';
				trans_r = (Uint8)eatFirstInt(infile.val, ',');
				trans_g = (Uint8)eatFirstInt(infile.val, ',');
				trans_b = (Uint8)eatFirstInt(infile.val, ',');
				
			}
			else if (infile.key == "animation") {
				int frame = 0;
				int TILE_ID = atoi(infile.nextValue().c_str());
				anim[TILE_ID].pos[frame].x = atoi(infile.nextValue().c_str());
				anim[TILE_ID].pos[frame].y = atoi(infile.nextValue().c_str());
				anim[TILE_ID].frame_duration[frame] = atoi(infile.nextValue().c_str());

				string repeat_val = infile.nextValue();
				while (repeat_val != "") {
					frame++;
					anim[TILE_ID].frames++;
					if (anim[TILE_ID].frames > MAX_TILE_FRAMES) break;
					anim[TILE_ID].pos[frame].x = atoi(repeat_val.c_str());
					anim[TILE_ID].pos[frame].y = atoi(infile.nextValue().c_str());
					anim[TILE_ID].frame_duration[frame] = atoi(infile.nextValue().c_str());

					repeat_val = infile.nextValue();
				}
			}
		
		}
		infile.close();
		loadGraphics(img);
	}

	current_map = filename;
}

void TileSet::logic() {
	for (int i = 0; i < TILE_SET_MAX_TILES; i++) {
		if (anim[i].duration == anim[i].frame_duration[anim[i].current_frame-1] && (anim[i].frames > 1)) {
			tiles[i].src.x = anim[i].pos[anim[i].current_frame-1].x;
			tiles[i].src.y = anim[i].pos[anim[i].current_frame-1].y;
			anim[i].duration = 0;
			if (anim[i].current_frame == anim[i].frames) {
				anim[i].current_frame = 1;
			} else anim[i].current_frame++;
		} else if (anim[i].frames > 1) anim[i].duration++;
	}
}

TileSet::~TileSet() {
	SDL_FreeSurface(sprites);
}
