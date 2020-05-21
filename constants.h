#pragma once
#include <math.h>


// Debugging
const bool DEBUG = true;

// Window attributes
const short WINDOW_WIDTH = 1080;  // \ Actual output resolution
const short WINDOW_HEIGHT = 640;  // / Should be 4:3 or 16:9 resolutions

// NES dimensions
const short WINDOW_WIDTH_NES = 256;	  // \ Original NES's typical output resolution.
const short WINDOW_HEIGHT_NES = 224;  // / Note its 8:7 aspect ratio.
const short BLOCKSIZE_NES = 16;

// Scaling for the NES to fit output window, but stretched to 4:3
// TODO: Fix for output resolutions smaller than 4:3 by adapting horizontal size first instead of vertical
// TODO: Make screen scaling realtime to window change; will probably require moving to a .cpp files
const double SCREEN_SCALE_X = floor(WINDOW_HEIGHT / (WINDOW_HEIGHT_NES * 6.0/7.0));  // We first find the integer upscale such that the screen takes the most space vertically
const double SCREEN_SCALE_Y = SCREEN_SCALE_X * 6.0/7.0;  // The Y-axis is further scaled to make the screen 4:3
