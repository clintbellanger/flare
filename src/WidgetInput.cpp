#include "WidgetInput.h"

WidgetInput::WidgetInput(SDL_Surface* _screen, FontEngine *_font, InputState *_inp)
	: screen(_screen), font(_font), inp(_inp) {
	
	enabled = true;
	inFocus = false;
	pressed = false;
	max_characters = 20;
	
	loadGraphics("images/menus/input.png");

	// position
	pos.w = background->w;
	pos.h = background->h/2;
	
	cursor_frame = 0;
	
}

void WidgetInput::loadGraphics(string filename) {

	// load input background image
	background = IMG_Load((PATH_DATA + filename).c_str());

	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

}

void WidgetInput::logic() {

	if (checkClick()) {
		inFocus = true;
	}

	// if clicking elsewhere unfocus the text box
	if (inp->pressing[MAIN1]) {
		if (!isWithin(pos, inp->mouse)) {
			inFocus = false;
		}
	}

	if (inFocus) {

		// handle text input
		text += inp->inkeys;
		if (text.length() > max_characters) {
			text = text.substr(0, max_characters);
		}
			
		// handle backspaces
		if (!inp->lock[DELETE] && inp->pressing[DELETE]) {
			inp->lock[DELETE] = true;
			text = text.substr(0, text.length()-1);
		}

		// animate cursor
		// cursor visible one second, invisible the next
		cursor_frame++;
		if (cursor_frame == FRAMES_PER_SEC+FRAMES_PER_SEC) cursor_frame = 0;
		
	}

}

void WidgetInput::render() {
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = pos.w;
	src.h = pos.h;

	if (!inFocus)
		src.y = 0;
	else if (isWithin(pos, inp->mouse))
		src.y = pos.h;
	else
		src.y = pos.h;

	SDL_BlitSurface(background, &src, screen, &pos);

	if (!inFocus) {
		font->render(text, font_pos.x, font_pos.y, JUSTIFY_LEFT, screen, FONT_WHITE);
	}
	else {
		if (cursor_frame < FRAMES_PER_SEC) {
			font->render(text + "|", font_pos.x, font_pos.y, JUSTIFY_LEFT, screen, FONT_WHITE);
		}
		else {
			font->render(text, font_pos.x, font_pos.y, JUSTIFY_LEFT, screen, FONT_WHITE);		
		}
	}
}

void WidgetInput::setPosition(int x, int y) {
	pos.x = x;
	pos.y = y;
	
	font_pos.x = pos.x  + (font->getHeight()/3);
	font_pos.y = pos.y + (pos.h/2) - (font->getHeight()/2);
}

bool WidgetInput::checkClick() {

	// disabled buttons can't be clicked;
	if (!enabled) return false;

	// main button already in use, new click not allowed
	if (inp->lock[MAIN1]) return false;

	// main click released, so the button state goes back to unpressed
	if (pressed && !inp->lock[MAIN1]) {
		pressed = false;
		
		if (isWithin(pos, inp->mouse)) {
		
			// activate upon release
			return true;
		}
	}

	pressed = false;
	
	// detect new click
	if (inp->pressing[MAIN1]) {
		if (isWithin(pos, inp->mouse)) {
		
			inp->lock[MAIN1] = true;
			pressed = true;

		}
	}
	return false;
}

