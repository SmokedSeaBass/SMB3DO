#pragma once
#include <math.h>
#include <string>

// Console ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Debugging
const bool META_DEBUG = true;
#define META_VERSION "0.1.0"

// Window attributes
const short WINDOW_WIDTH = 1920;  // \ Actual output resolution
const short WINDOW_HEIGHT = 1080;  // / Should be 4:3 or 16:9 resolutions

// NES dimensions
const short WINDOW_WIDTH_NES = 256;	  // \ Original NES's typical output resolution.
const short WINDOW_HEIGHT_NES = 224;  // / Note its 8:7 aspect ratio.
const short BLOCKSIZE_NES = 16;

// Scaling for the NES to fit output window, but stretched to 4:3
// TODO: Fix for output resolutions smaller than 4:3 by conditionally adapting horizontal size first instead of vertical
// TODO: Make screen scaling realtime to window change; will probably require moving to a .cpp files
const double SCREEN_SCALE_X = floor(WINDOW_HEIGHT / (WINDOW_HEIGHT_NES * 6.0 / 7.0));  // We first find the integer upscale such that the screen takes the most space vertically
const double SCREEN_SCALE_Y = SCREEN_SCALE_X * 6.0/7.0;  // The Y-axis is further scaled to make the screen 4:3

// SDL_Color constants
const SDL_Color SDL_COLOR_BLACK = { 0x00, 0x00, 0x00, 0xFF };
const SDL_Color SDL_COLOR_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
const SDL_Color SDL_COLOR_RED = { 0xFF, 0x00, 0x00, 0xFF };
const SDL_Color SDL_COLOR_GREEN = { 0x00, 0xFF, 0x00, 0xFF };
const SDL_Color SDL_COLOR_BLUE = { 0x00, 0x00, 0xFF, 0xFF };