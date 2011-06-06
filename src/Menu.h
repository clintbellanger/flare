#ifndef MENU_H
#define MENU_H

/**
 * class MenuInventory
 *
 * The base class for Menu objects
 *
 * @author kitano
 * @license GPL
 */

#include "SDL_image.h"
#include "FontEngine.h"
#include "InputState.h"

class Menu {
protected:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;

	SDL_Surface *background;
	SDL_Rect window_area;

	bool visible;

public:
	Menu(SDL_Surface*, InputState*, FontEngine*);

	bool isVisible() { return visible; }
	void setVisible(bool _visible) { visible = _visible; }

	virtual void render() = 0;
};

#endif

