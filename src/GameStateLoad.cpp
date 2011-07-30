/**
 * GameStateLoad
 * 
 * @author Clint Bellanger
 * @license GPL
 */
#include "GameStateLoad.h"
#include "GameStateTitle.h"
#include "GameStatePlay.h"
#include "GameStateNew.h"
#include "MenuConfirm.h"

GameStateLoad::GameStateLoad(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg) : GameState(_screen, _inp, _font, _msg) {
	items = new ItemDatabase(screen, font, msg);
	portrait = NULL;
	loading_requested = false;
	loading = false;
	loaded = false;
	
	// Confirmation box to confirm deleting
	confirm = new MenuConfirm(screen, inp, font, msg->get("delete_save_button"), msg->get("delete_save_dialog"));
	button_exit = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	button_exit->label = msg->get("exit_to_title_button");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;	
	
	button_action = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	button_action->label = msg->get("choose_slot_button");
	button_action->enabled = false;
	button_action->pos.x = (VIEW_W - 640)/2 + 480 - button_action->pos.w/2;
	button_action->pos.y = (VIEW_H - 480)/2 + 384;
	
	button_alternate = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	button_alternate->label = msg->get("delete_save_button");
	button_alternate->enabled = false;
	button_alternate->pos.x = (VIEW_W - 640)/2 + 480 - button_alternate->pos.w/2;
	button_alternate->pos.y = (VIEW_H - 480)/2 + 415;
	load_game = false;
	
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		sprites[i] = NULL;
		current_map[i] = "";
	}
	
	loadGraphics();
	readGameSlots();
	
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		slot_pos[i].x = (VIEW_W - 640)/2;
		slot_pos[i].y = (VIEW_H - 480)/2 + (i * 96) + 32;
		slot_pos[i].w = 288;
		slot_pos[i].h = 96;
	}
	
	selected_slot = -1;
	
	// label positions within each slot
	name_pos.x = 16;
	name_pos.y = 16;

	level_pos.x = 24;
	level_pos.y = 40;

	map_pos.x = 24;
	map_pos.y = 56;

	sprites_pos.x = 178;
	sprites_pos.y = -24;
	
	// temp
	current_frame = 0;
	frame_ticker = 0;
}

void GameStateLoad::loadGraphics() {
	background = NULL;
	selection = NULL;
	portrait_border = NULL;
	
	background = IMG_Load((PATH_DATA + "images/menus/game_slots.png").c_str());
	selection = IMG_Load((PATH_DATA + "images/menus/game_slot_select.png").c_str());
	portrait_border = IMG_Load((PATH_DATA + "images/menus/portrait_border.png").c_str());
	if(!background || !selection || !portrait_border) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	SDL_SetColorKey( selection, SDL_SRCCOLORKEY, SDL_MapRGB(selection->format, 255, 0, 255) ); 
	SDL_SetColorKey( portrait_border, SDL_SRCCOLORKEY, SDL_MapRGB(portrait_border->format, 255, 0, 255) ); 
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);
	
	cleanup = selection;
	selection = SDL_DisplayFormatAlpha(selection);
	SDL_FreeSurface(cleanup);
	
	cleanup = portrait_border;
	portrait_border = SDL_DisplayFormatAlpha(portrait_border);
	SDL_FreeSurface(cleanup);
	
}

void GameStateLoad::loadPortrait(int slot) {
	SDL_FreeSurface(portrait);
	portrait = NULL;
	
	if (stats[slot].name == "") return;
	
	portrait = IMG_Load((PATH_DATA + "images/portraits/" + stats[slot].portrait + ".png").c_str());
	if (!portrait) return;
	
	// optimize
	SDL_Surface *cleanup = portrait;
	portrait = SDL_DisplayFormatAlpha(portrait);
	SDL_FreeSurface(cleanup);
}

void GameStateLoad::readGameSlots() {
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		readGameSlot(i);
	}
}

string GameStateLoad::getMapName(string map_filename) {
	FileParser infile;
	if (!infile.open(PATH_DATA + "maps/" + map_filename)) return "";
	string map_name = "";
	
	while (map_name == "" && infile.next()) {
		if (infile.key == "title")
			map_name = infile.val;
	}
	
	infile.close();
	return map_name;
}

void GameStateLoad::readGameSlot(int slot) {

	stringstream filename;
	FileParser infile;
	
	// abort if not a valid slot number
	if (slot < 0 || slot >= GAME_SLOT_MAX) return;

	// save slots are named save#.txt
	filename << "save" << (slot+1) << ".txt";

	if (!infile.open(PATH_USER + filename.str())) return;
	
	while (infile.next()) {
	
		// load (key=value) pairs
		if (infile.key == "name")
			stats[slot].name = infile.val;
		else if (infile.key == "xp")
			stats[slot].xp = atoi(infile.val.c_str());
		else if (infile.key == "build") {
			stats[slot].physical_character = atoi(infile.nextValue().c_str());
			stats[slot].mental_character = atoi(infile.nextValue().c_str());
			stats[slot].offense_character = atoi(infile.nextValue().c_str());
			stats[slot].defense_character = atoi(infile.nextValue().c_str());
		}
		else if (infile.key == "equipped") {
			equipped[slot][0] = atoi(infile.nextValue().c_str());
			equipped[slot][1] = atoi(infile.nextValue().c_str());
			equipped[slot][2] = atoi(infile.nextValue().c_str());
		}
		else if (infile.key == "option") {
			stats[slot].base = infile.nextValue();
			stats[slot].head = infile.nextValue();
			stats[slot].portrait = infile.nextValue();
		}
		else if (infile.key == "spawn") {
			current_map[slot] = getMapName(infile.nextValue());
		}
	}
	infile.close();
	
	stats[slot].recalc();
	loadPreview(slot);

}

void GameStateLoad::loadPreview(int slot) {

	string img_main;
	string img_body;
	string img_off;

	SDL_Surface *gfx_body = NULL;
	SDL_Surface *gfx_main = NULL;
	SDL_Surface *gfx_off = NULL;
	SDL_Surface *gfx_head = NULL;
	SDL_Rect src;
	SDL_Rect dest;
	
	if (equipped[slot][0] != 0)	img_main = items->items[equipped[slot][0]].gfx;
	if (equipped[slot][1] != 0)	img_body = items->items[equipped[slot][1]].gfx;
	else img_body = "clothes";
	if (equipped[slot][2] != 0)	img_off = items->items[equipped[slot][2]].gfx;
	
	if (sprites[slot]) SDL_FreeSurface(sprites[slot]);	
	sprites[slot] = IMG_Load((PATH_DATA + "images/avatar/preview_background.png").c_str());
	SDL_SetColorKey(sprites[slot], SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 

	// optimize
	SDL_Surface *cleanup = sprites[slot];
	sprites[slot] = SDL_DisplayFormatAlpha(sprites[slot]);
	SDL_FreeSurface(cleanup);
	
	// composite the hero graphic
	
	if (img_body != "") gfx_body = IMG_Load((PATH_DATA + "images/avatar/" + stats[slot].base + "/" + img_body + ".png").c_str());
	if (img_main != "") gfx_main = IMG_Load((PATH_DATA + "images/avatar/" + stats[slot].base + "/" + img_main + ".png").c_str());
	if (img_off != "") gfx_off = IMG_Load((PATH_DATA + "images/avatar/" + stats[slot].base + "/" + img_off + ".png").c_str());
	gfx_head = IMG_Load((PATH_DATA + "images/avatar/" + stats[slot].base + "/" + stats[slot].head + ".png").c_str());

	if (gfx_body) SDL_SetColorKey(gfx_body, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_main) SDL_SetColorKey(gfx_main, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_off) SDL_SetColorKey(gfx_off, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_head) SDL_SetColorKey(gfx_head, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	
	src.w = dest.w = 512; // for this menu we only need the stance animation
	src.h = dest.h = 128; // for this menu we only need one direction
	src.x = dest.x = 0;
	src.y = 768; // for this meny we only need facing down
	dest.y = 0;
	
	if (gfx_body) SDL_BlitSurface(gfx_body, &src, sprites[slot], &dest);
	if (gfx_main) SDL_BlitSurface(gfx_main, &src, sprites[slot], &dest);
	if (gfx_head) SDL_BlitSurface(gfx_head, &src, sprites[slot], &dest);	
	if (gfx_off) SDL_BlitSurface(gfx_off, &src, sprites[slot], &dest);

	if (gfx_body) SDL_FreeSurface(gfx_body);
	if (gfx_main) SDL_FreeSurface(gfx_main);
	if (gfx_head) SDL_FreeSurface(gfx_head);
	if (gfx_off) SDL_FreeSurface(gfx_off);

}


void GameStateLoad::logic() {

	frame_ticker++;
	if (frame_ticker == 64) frame_ticker = 0;
	if (frame_ticker < 32)
		current_frame = frame_ticker / 8;
	else
		current_frame = (63 - frame_ticker) / 8;

	if (button_exit->checkClick()) {
		requestedGameState = new GameStateTitle(screen, inp, font, msg);
	}
	
	if(loading_requested) {
		loading = true;
		loading_requested = false;
		logicLoading();
	}

	if (button_action->checkClick()) {
		if (stats[selected_slot].name == "") {
			// create a new game
			GameStateNew* newgame = new GameStateNew(screen, inp, font, msg);
			newgame->game_slot = selected_slot + 1;
			requestedGameState = newgame;
		}
		else {
			loading_requested = true;
		}
	}
	if (button_alternate->checkClick())
	  {
	    // Display pop-up to make sure save should be deleted
	    confirm->visible = true;
	    confirm->render();
	  }
	if (confirm->visible){
	  confirm->logic();
	  if(confirm->isConfirmRequested()){
	    stringstream filename;
	    filename << PATH_USER << "save" << (selected_slot+1) << ".txt";
	    if(remove(filename.str().c_str()) != 0)
	      perror("Error deleting save from path");
	    stats[selected_slot] = StatBlock();
	    readGameSlot(selected_slot);
	    loadPreview(selected_slot);
	    loadPortrait(selected_slot);
	    button_alternate->enabled = false;
	    button_action->label = msg->get("new_game_button");
	    confirm->visible = false;
	  }
	}
	// check clicking game slot
	if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) {
		for (int i=0; i<GAME_SLOT_MAX; i++) {
			if (isWithin(slot_pos[i], inp->mouse)) {
				selected_slot = i;
				inp->lock[MAIN1] = true;
				loadPortrait(selected_slot);
				
				button_action->enabled = true;
				if (stats[selected_slot].name == "") {
					button_action->label = msg->get("new_game_button");
					button_alternate->enabled = false;
				}
				else {
					button_action->label = msg->get("load_game_button");
					button_alternate->enabled = true;
				}
			}
		}
	}
}

void GameStateLoad::logicLoading() {
	// load an existing game
	GameStatePlay* play = new GameStatePlay(screen, inp, font, msg);
	play->resetGame();
	play->game_slot = selected_slot + 1;
	play->loadGame();
	requestedGameState = play;
	loaded = true;
	loading = false;
}

void GameStateLoad::render() {

	SDL_Rect src;
	SDL_Rect dest;

	// display buttons
	button_exit->render();
	button_action->render();
	button_alternate->render();

	// display background
	src.w = 288;
	src.h = 384;
	src.x = src.y = 0;
	dest.x = slot_pos[0].x;
	dest.y = slot_pos[0].y;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// display selection
	if (selected_slot >= 0) {
		src.w = 288;
		src.h = 96;
		src.x = src.y = 0;
		SDL_BlitSurface(selection, &src, screen, &slot_pos[selected_slot]);	
	}
	

	// portrait
	if (selected_slot >= 0 && portrait != NULL) {

		src.w = src.h = 320;
		dest.x = VIEW_W_HALF;
		dest.y = (VIEW_H - 480)/2 + 32;

		SDL_BlitSurface(portrait, &src, screen, &dest);
		SDL_BlitSurface(portrait_border, &src, screen, &dest);
	}
	
	Point label;
	stringstream ss;

	if( loading_requested || loading || loaded ) {
		label.x = button_action->pos.x + ( button_action->pos.w / 2 );
		label.y = button_action->pos.y - button_action->pos.h + 10;
		if ( loaded ) {
			font->render(msg->get("entering_game_status"), label.x, label.y, JUSTIFY_CENTER, screen, FONT_WHITE);
		} else {
			font->render(msg->get("loading_game_status"), label.x, label.y, JUSTIFY_CENTER, screen, FONT_WHITE);
		}
	}
	
	// display text
	for (int slot=0; slot<GAME_SLOT_MAX; slot++) {
		if (stats[slot].name != "") {
		
			// name
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;		
			font->render(stats[slot].name, label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// level
			ss.str("");
			label.x = slot_pos[slot].x + level_pos.x;
			label.y = slot_pos[slot].y + level_pos.y;		
			ss << msg->get("character_level_class", stats[slot].level, stats[slot].character_class);
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);
			
			// map
			label.x = slot_pos[slot].x + map_pos.x;
			label.y = slot_pos[slot].y + map_pos.y;		
			font->render(current_map[slot], label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// render character preview
			dest.x = slot_pos[slot].x + sprites_pos.x;
			dest.y = slot_pos[slot].y + sprites_pos.y;
			src.x = current_frame * 128;
			src.y = 0;
			src.w = src.h = 128;
			
			SDL_BlitSurface(sprites[slot], &src, screen, &dest);
			
		}
		else {
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;		
			font->render(msg->get("empty_slot"), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);
		}
	}
	// display warnings
	if(confirm->visible) confirm->render();
}

GameStateLoad::~GameStateLoad() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(selection);
	SDL_FreeSurface(portrait_border);
	SDL_FreeSurface(portrait);
	delete button_exit;
	delete button_action;
	delete button_alternate;
	delete items;
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		SDL_FreeSurface(sprites[i]);
	}
}
