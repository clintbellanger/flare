/**
 * class MapIso
 *
 * Isometric map data structure and rendering
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#include "MapIso.h"
#include "FileParser.h"

MapIso::MapIso(SDL_Surface *_screen, CampaignManager *_camp) {

	screen = _screen;
	camp = _camp;

	// cam(x,y) is where on the map the camera is pointing
	// units found in Settings.h (UNITS_PER_TILE)
	cam.x = 0;
	cam.y = 0;
	
	new_music = false;

	clearEvents();
	enemy_awaiting_queue = false;
	npc_awaiting_queue = false;
	clearEnemy(new_enemy);
	clearNPC(new_npc);
	
	sfx = NULL;
	sfx_filename = "";
	music = NULL;
	log_msg = "";
	shaky_cam_ticks = 0;
	
	// spawn is a special map that defines where the campaign begins
	// load("spawn.txt");
}



void MapIso::clearEvents() {
	for (int i=0; i<256; i++) {
		events[i].type = "";
		events[i].location.x = 0;
		events[i].location.y = 0;
		events[i].location.w = 0;
		events[i].location.h = 0;
		events[i].comp_num = 0;
		for (int j=0; j<8; j++) {
			events[i].components[j].type = "";
			events[i].components[j].s = "";
			events[i].components[j].x = 0;
			events[i].components[j].y = 0;
			events[i].components[j].z = 0;
		}
	}
	event_count = 0;
}

void MapIso::removeEvent(int eid) {
	for (int i=eid; i<event_count; i++) {
		if (i<256) {
			events[i] = events[i+1];
		}
	}
	event_count--;
}

void MapIso::clearEnemy(Map_Enemy e) {
	e.pos.x = 0;
	e.pos.y = 0;
	e.direction = 0;
	e.type = "";
}

void MapIso::clearNPC(Map_NPC n) {
	n.id = "";
	n.pos.x = 0;
	n.pos.y = 0;
}

void MapIso::playSFX(string filename) {
	// only load from file if the requested soundfx isn't already loaded
	if (filename != sfx_filename) {
		if (sfx) Mix_FreeChunk(sfx);
		sfx = Mix_LoadWAV(filename.c_str());
		sfx_filename = filename;
	}
	if (sfx) Mix_PlayChannel(-1, sfx, 0);	
}


/**
 * load
 */
int MapIso::load(string filename) {
	FileParser infile;
	string val;
	string cur_layer;
	string data_format;
  
	clearEvents();
  
    event_count = 0;
  
	if (infile.open(("maps/" + filename).c_str())) {
		while (infile.next()) {
			if (infile.new_section) {
				data_format = "dec"; // default
				
				if (enemy_awaiting_queue) {
					enemies.push(new_enemy);
					enemy_awaiting_queue = false;
				}
				if (npc_awaiting_queue) {
					npcs.push(new_npc);
					npc_awaiting_queue = false;
				}
				
				// for sections that are stored in collections, add a new object here
				if (infile.section == "enemy") {
					clearEnemy(new_enemy);
					enemy_awaiting_queue = true;
				}
				else if (infile.section == "npc") {
					clearNPC(new_npc);
					npc_awaiting_queue = true;
				}
				else if (infile.section == "event") {
					event_count++;
				}
				
			}
			if (infile.section == "header") {
				if (infile.key == "title") {
					this->title = infile.val;
				}
				else if (infile.key == "width") {
					this->w = atoi(infile.val.c_str());
				}
				else if (infile.key == "height") {
					this->h = atoi(infile.val.c_str());
				}
				else if (infile.key == "tileset") {
					this->tileset = infile.val;
				}
				else if (infile.key == "music") {
					if (this->music_filename == infile.val) {
						this->new_music = false;
					}
					else {
						this->music_filename = infile.val;
						this->new_music = true;
					}
				}
				else if (infile.key == "spawnpoint") {
					spawn.x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					spawn.y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					spawn_dir = atoi(infile.nextValue().c_str());
				}
			}
			else if (infile.section == "layer") {
				if (infile.key == "id") {
					cur_layer = infile.val;
				}
				else if (infile.key == "format") {
					data_format = infile.val;
				}
				else if (infile.key == "data") {
					// layer map data handled as a special case

					// The next h lines must contain layer data.  TODO: err
					if (data_format == "hex") {
						for (int j=0; j<h; j++) {
							val = infile.getRawLine() + ',';
							for (int i=0; i<w; i++) {
								if (cur_layer == "background") background[i][j] = eatFirstHex(val, ',');
								else if (cur_layer == "object") object[i][j] = eatFirstHex(val, ',');
								else if (cur_layer == "collision") collision[i][j] = eatFirstHex(val, ',');
							}
						}
					}
					else if (data_format == "dec") {
						for (int j=0; j<h; j++) {
							val = infile.getRawLine() + ',';
							for (int i=0; i<w; i++) {
								if (cur_layer == "background") background[i][j] = eatFirstInt(val, ',');
								else if (cur_layer == "object") object[i][j] = eatFirstInt(val, ',');
								else if (cur_layer == "collision") collision[i][j] = eatFirstInt(val, ',');
							}
						}
					}
				}
			}
			else if (infile.section == "enemy") {
				if (infile.key == "type") {
					new_enemy.type = infile.val;
				}
				else if (infile.key == "spawnpoint") {
					new_enemy.pos.x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					new_enemy.pos.y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					new_enemy.direction = atoi(infile.nextValue().c_str());
				}
			}
			else if (infile.section == "npc") {
				if (infile.key == "id") {
					new_npc.id = infile.val;
				}
				else if (infile.key == "position") {
					new_npc.pos.x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					new_npc.pos.y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}
			}
			else if (infile.section == "event") {
				if (infile.key == "type") {
					events[event_count-1].type = infile.val;
				}
				else if (infile.key == "location") {
					events[event_count-1].location.x = atoi(infile.nextValue().c_str());
					events[event_count-1].location.y = atoi(infile.nextValue().c_str());
					events[event_count-1].location.w = atoi(infile.nextValue().c_str());
					events[event_count-1].location.h = atoi(infile.nextValue().c_str());
				}
				else {
					// new event component
					Event_Component *e = &events[event_count-1].components[events[event_count-1].comp_num];
					e->type = infile.key;
					
					if (infile.key == "intermap") {
						e->s = infile.nextValue();
						e->x = atoi(infile.nextValue().c_str());
						e->y = atoi(infile.nextValue().c_str());
					}
					else if (infile.key == "mapmod") {
						e->s = infile.nextValue();
						e->x = atoi(infile.nextValue().c_str());
						e->y = atoi(infile.nextValue().c_str());
						e->z = atoi(infile.nextValue().c_str());
					}
					else if (infile.key == "soundfx") {
						e->s = infile.val;
					}
					else if (infile.key == "loot") {
						e->s = infile.nextValue();
						e->x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
						e->y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
						e->z = atoi(infile.nextValue().c_str());
					}
					else if (infile.key == "msg") {
						e->s = infile.val;
					}
					else if (infile.key == "shakycam") {
						e->x = atoi(infile.val.c_str());
					}
					else if (infile.key == "requires_status") {
						e->s = infile.val;
					}
					else if (infile.key == "requires_not") {
						e->s = infile.val;
					}
					else if (infile.key == "requires_item") {
						e->x = atoi(infile.val.c_str());
					}
					else if (infile.key == "set_status") {
						e->s = infile.val;
					}
					else if (infile.key == "unset_status") {
						e->s = infile.val;
					}
					else if (infile.key == "remove_item") {
						e->x = atoi(infile.val.c_str());
					}
					else if (infile.key == "reward_xp") {
						e->x = atoi(infile.val.c_str());
					}
					
					events[event_count-1].comp_num++;
				}
			}
		}

		infile.close();
		
		// reached end of file.  Handle any final sections.
		if (enemy_awaiting_queue) {
			enemies.push(new_enemy);
			enemy_awaiting_queue = false;
		}
		if (npc_awaiting_queue) {
			npcs.push(new_npc);
			npc_awaiting_queue = false;
		}
	}

	collider.setmap(collision);
	collider.map_size.x = w;
	collider.map_size.y = h;
	
	if (this->new_music) {
		loadMusic();
		this->new_music = false;
	}
	tset.load(this->tileset);

	return 0;
}

void MapIso::loadMusic() {

	if (music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}
	music = Mix_LoadMUS(("music/" + this->music_filename).c_str());
	if (!music) {
	  printf("Mix_LoadMUS: %s\n", Mix_GetError());
	  SDL_Quit();
	}

	Mix_VolumeMusic(MUSIC_VOLUME);
	Mix_PlayMusic(music, -1);
	
}

void MapIso::logic() {
	if (shaky_cam_ticks > 0) shaky_cam_ticks--;
}

void MapIso::render(Renderable r[], int rnum) {

	// r will become a list of renderables.  Everything not on the map already:
	// - hero
	// - npcs
	// - monsters
	// - loot
	// - special effects
	// we want to sort these by map draw order.  Then, we use a cursor to move through the 
	// renderables while we're also moving through the map tiles.  After we draw each map tile we
	// check to see if it's time to draw the next renderable yet.

	short unsigned int i;
	short unsigned int j;
	//SDL_Rect src;
	SDL_Rect dest;
	int current_tile;
	
	Point xcam;
	Point ycam;
	
	if (shaky_cam_ticks == 0) {
		xcam.x = cam.x/UNITS_PER_PIXEL_X;
		xcam.y = cam.y/UNITS_PER_PIXEL_X;
		ycam.x = cam.x/UNITS_PER_PIXEL_Y;
		ycam.y = cam.y/UNITS_PER_PIXEL_Y;
	}
	else {
		xcam.x = (cam.x + rand() % 16 - 8) /UNITS_PER_PIXEL_X;
		xcam.y = (cam.y + rand() % 16 - 8) /UNITS_PER_PIXEL_X;
		ycam.x = (cam.x + rand() % 16 - 8) /UNITS_PER_PIXEL_Y;
		ycam.y = (cam.y + rand() % 16 - 8) /UNITS_PER_PIXEL_Y;
	}
	
	// todo: trim by screen rect
	// background
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++) {
		  
			current_tile = background[i][j];
			
			if (current_tile > 0) {			
			
				dest.x = VIEW_W_HALF + (i * TILE_W_HALF - xcam.x) - (j * TILE_W_HALF - xcam.y);
				dest.y = VIEW_H_HALF + (i * TILE_H_HALF - ycam.x) + (j * TILE_H_HALF - ycam.y) + TILE_H_HALF;
				// adding TILE_H_HALF gets us to the tile center instead of top corner
				dest.x -= tset.tiles[current_tile].offset.x;
				dest.y -= tset.tiles[current_tile].offset.y;
				dest.w = tset.tiles[current_tile].src.w;
				dest.h = tset.tiles[current_tile].src.h;
				
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
	
			}
		}
	}

	// some renderables are drawn above the background and below the objects
	for (int ri = 0; ri < rnum; ri++) {			
		if (!r[ri].object_layer) {
				
			// draw renderable
			dest.w = r[ri].src.w;
			dest.h = r[ri].src.h;
			dest.x = VIEW_W_HALF + (r[ri].map_pos.x/UNITS_PER_PIXEL_X - xcam.x) - (r[ri].map_pos.y/UNITS_PER_PIXEL_X - xcam.y) - r[ri].offset.x;
			dest.y = VIEW_H_HALF + (r[ri].map_pos.x/UNITS_PER_PIXEL_Y - ycam.x) + (r[ri].map_pos.y/UNITS_PER_PIXEL_Y - ycam.y) - r[ri].offset.y;

			SDL_BlitSurface(r[ri].sprite, &r[ri].src, screen, &dest);
		} 
	}
		
	int r_cursor = 0;

	// todo: trim by screen rect
	// object layer
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++) {
		
			current_tile = object[i][j];
			
			if (current_tile > 0) {			
			
				dest.x = VIEW_W_HALF + (i * TILE_W_HALF - xcam.x) - (j * TILE_W_HALF - xcam.y);
				dest.y = VIEW_H_HALF + (i * TILE_H_HALF - ycam.x) + (j * TILE_H_HALF - ycam.y) + TILE_H_HALF;
				// adding TILE_H_HALF gets us to the tile center instead of top corner
				dest.x -= tset.tiles[current_tile].offset.x;
				dest.y -= tset.tiles[current_tile].offset.y;
				dest.w = tset.tiles[current_tile].src.w;
				dest.h = tset.tiles[current_tile].src.h;
				
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
	
			}
			
			// some renderable entities go in this layer
			while (r_cursor < rnum && r[r_cursor].tile.x == i && r[r_cursor].tile.y == j) {
				if (r[r_cursor].object_layer) {
					// draw renderable
					dest.w = r[r_cursor].src.w;
					dest.h = r[r_cursor].src.h;
					dest.x = VIEW_W_HALF + (r[r_cursor].map_pos.x/UNITS_PER_PIXEL_X - xcam.x) - (r[r_cursor].map_pos.y/UNITS_PER_PIXEL_X - xcam.y) - r[r_cursor].offset.x;
					dest.y = VIEW_H_HALF + (r[r_cursor].map_pos.x/UNITS_PER_PIXEL_Y - ycam.x) + (r[r_cursor].map_pos.y/UNITS_PER_PIXEL_Y - ycam.y) - r[r_cursor].offset.y;

					SDL_BlitSurface(r[r_cursor].sprite, &r[r_cursor].src, screen, &dest);				
				}
				
				r_cursor++;

			}
		}
	}
}

void MapIso::checkEvents(Point loc) {
	Point maploc;
	maploc.x = loc.x >> TILE_SHIFT;
	maploc.y = loc.y >> TILE_SHIFT;
	for (int i=0; i<event_count; i++) {
		if (maploc.x >= events[i].location.x &&
		    maploc.y >= events[i].location.y &&
		    maploc.x <= events[i].location.x + events[i].location.w-1 &&
			maploc.y <= events[i].location.y + events[i].location.h-1) {
			executeEvent(i);
		}
	}
}

/**
 * A particular event has been triggered.
 * Process all of this event's components.
 *
 * @param eid The triggered event id
 */
void MapIso::executeEvent(int eid) {
	Event_Component *ec;
	for (int i=0; i<events[eid].comp_num; i++) {
		ec = &events[eid].components[i];
		
		if (ec->type == "requires_status") {
			if (!camp->checkStatus(ec->s)) return;
		}
		else if (ec->type == "requires_not") {
			if (camp->checkStatus(ec->s)) return;
		}
		else if (ec->type == "requires_item") {
			if (!camp->checkItem(ec->x)) return;
		}
		else if (ec->type == "set_status") {
			camp->setStatus(ec->s);
		}
		else if (ec->type == "unset_status") {
			camp->unsetStatus(ec->s);
		}
		if (ec->type == "intermap") {
			teleportation = true;
			teleport_mapname = ec->s;
			teleport_destination.x = ec->x * UNITS_PER_TILE + UNITS_PER_TILE/2;
			teleport_destination.y = ec->y * UNITS_PER_TILE + UNITS_PER_TILE/2;
		}
		else if (ec->type == "mapmod") {
			if (ec->s == "collision") {
				collision[ec->x][ec->y] = ec->z;
				collider.colmap[ec->x][ec->y] = ec->z;
			}
			else if (ec->s == "object") {
				object[ec->x][ec->y] = ec->z;			
			}
			else if (ec->s == "background") {
				background[ec->x][ec->y] = ec->z;			
			}
		}
		else if (ec->type == "soundfx") {
			playSFX(ec->s);
		}
		else if (ec->type == "loot") {
			loot.push(*ec);
		}
		else if (ec->type == "msg") {
			log_msg = ec->s;
		}
		else if (ec->type == "shakycam") {
			shaky_cam_ticks = ec->x;
		}
		else if (ec->type == "remove_item") {
			camp->removeItem(ec->x);
		}
		else if (ec->type == "reward_xp") {
			camp->rewardXP(ec->x);
		}
	}
	if (events[eid].type == "run_once") {
		removeEvent(eid);
	}
}

MapIso::~MapIso() {
	if (music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
	}
	if (sfx) Mix_FreeChunk(sfx);
}

