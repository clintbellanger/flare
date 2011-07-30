/**
 * class MenuExit
 *
 * @author kitano
 * @license GPL
 */

#include "MenuExit.h"

MenuExit::MenuExit(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg) : Menu(_screen, inp = _inp, _font) {
	msg = _msg;

	exitClicked = false;

	window_area.w = 192;
	window_area.h = 64;
	window_area.x = (VIEW_W/2) - (window_area.w/2);
	window_area.y = (VIEW_H - window_area.h)/2;
	
	buttonExit = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	buttonExit->label = "Exit";
	buttonExit->pos.x = VIEW_W_HALF - buttonExit->pos.w/2;
	buttonExit->pos.y = VIEW_H/2;

	buttonClose = new WidgetButton(screen, font, inp, "images/menus/buttons/button_x.png");
	buttonClose->pos.x = window_area.x + window_area.w;
	buttonClose->pos.y = window_area.y;

	loadGraphics();
}

void MenuExit::loadGraphics() {
	background = IMG_Load((PATH_DATA + "images/menus/confirm_bg.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
}

void MenuExit::logic() {
	if (visible) {
		if (buttonExit->checkClick()) {
			exitClicked = true;	
		}
		if (buttonClose->checkClick()) {
			visible = false;
		}
	}
}

void MenuExit::render() {
	SDL_Rect src;

	// background
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &window_area);

	font->render(msg->get("exit_dialog"), window_area.x + window_area.w/2, window_area.y + 10, JUSTIFY_CENTER, screen, FONT_WHITE);

	buttonExit->render();
	buttonClose->render();
}

MenuExit::~MenuExit() {
	delete buttonExit;
	delete buttonClose;
	SDL_FreeSurface(background);
}

