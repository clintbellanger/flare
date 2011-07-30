/**
 * class MenuPowers
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "MenuTooltip.h"
#include "PowerManager.h"
#include "WidgetButton.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

using namespace std;

class MenuPowers {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;
	StatBlock *stats;
	PowerManager *powers;
	MessageEngine *msg;

	SDL_Surface *background;
	SDL_Surface *powers_step;
	SDL_Surface *powers_unlock;
	WidgetButton *closeButton;
	
	void loadGraphics();
	void displayBuild(int value, int x);

public:
	MenuPowers(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats, PowerManager *_powers, MessageEngine *_msg);
	~MenuPowers();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	
	bool visible;
	SDL_Rect slots[20]; // the location of power slots

};

#endif
