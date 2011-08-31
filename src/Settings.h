/**
 * Settings
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

// Path info
extern std::string PATH_CONF; // user-configurable settings files
extern std::string PATH_USER; // important per-user data (saves)
extern std::string PATH_DATA; // common game data
extern std::string PATH_MOD; // language code

// Audio and Video Settings
extern int MUSIC_VOLUME;
extern int SOUND_VOLUME;
extern bool FULLSCREEN;
extern int FRAMES_PER_SEC;
extern int VIEW_W;
extern int VIEW_H;
extern int VIEW_W_HALF;
extern int VIEW_H_HALF;
extern bool DOUBLEBUF;
extern bool HWSURFACE;

// Input Settings
extern bool MOUSE_MOVE;

// Engine Settings
extern bool MENUS_PAUSE;

// Tile Settings
extern int UNITS_PER_TILE;
extern int TILE_SHIFT;
extern int UNITS_PER_PIXEL_X;
extern int UNITS_PER_PIXEL_Y;
extern int TILE_W;
extern int TILE_H;
extern int TILE_W_HALF;
extern int TILE_H_HALF;

void setPaths();
bool loadSettings();
bool saveSettings();

#endif
