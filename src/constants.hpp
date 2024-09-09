/**
 *  \file constants.hpp
 *
 *  Include file for global constants and defines.
*/

#pragma once

#include <SDL3/SDL.h>

#include <math.h>
#include "units.hpp"

/* Debugging */
constexpr bool META_DEBUG = true;
constexpr char* META_VERSION = "0.3.0";

/* Render window attributes */
//const unsigned int WINDOW_WIDTH = 1280;  // \ Actual output resolution
//const unsigned int WINDOW_HEIGHT = 720;  // / Should be 4:3 or 16:9 resolutions
//const double FPS_LIMIT = 60;  // Set to -1 to uncap
//const double DELTA_TIME = 60.0 / FPS_LIMIT;

/* NES screen dimensions */
constexpr Units::Pixels NES_WINDOW_WIDTH = 256;	// \ Original NES's typical output resolution.
constexpr Units::Pixels NES_WINDOW_HEIGHT = 224;	// / Note its 8:7 aspect ratio.
constexpr Units::Game TILESIZE_NES = 16;		// In NES pixels

/* Scaling for the NES to fit output window, but stretched to 4:3 to emulate CRT stretching*/
// TODO: Fix for output resolutions smaller than 4:3 by conditionally adapting horizontal size first instead of vertical
// TODO: Make screen scaling realtime to window change; will probably require moving to a .cpp file
//const double SCREEN_SCALE_X = floor(WINDOW_HEIGHT / (WINDOW_HEIGHT_NES * 6.0 / 7.0));  // We first find the integer upscale such that the NES screen takes the most space vertically
//const double SCREEN_SCALE_Y = SCREEN_SCALE_X * 6.0 / 7.0;  // The Y-axis is distorted to make the screen 4:3

/* SDL_Color constants */
const SDL_Color SDL_COLOR_BLACK = { 0x00, 0x00, 0x00, 0xFF };
const SDL_Color SDL_COLOR_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
const SDL_Color SDL_COLOR_RED = { 0xFF, 0x00, 0x00, 0xFF };
const SDL_Color SDL_COLOR_GREEN = { 0x00, 0xFF, 0x00, 0xFF };
const SDL_Color SDL_COLOR_BLUE = { 0x00, 0x00, 0xFF, 0xFF };

/* Some math help */
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}