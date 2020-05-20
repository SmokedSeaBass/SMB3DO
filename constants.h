#pragma once
#include <math.h>

// Window attributes
const short WINDOW_WIDTH = 512;				// \ Actual output resolution
const short WINDOW_HEIGHT = 448;			// / Should be 4:3 or 16:9 resolutions

// SNES dimensions
const short WINDOW_WIDTH_SNES = 256;		// \ Original SNES's typical output resolution.
const short WINDOW_HEIGHT_SNES = 224;		// / Note its 8:7 aspect ratio.


const unsigned char SCREEN_SCALE = 1 << static_cast<int>(floor(log2(WINDOW_HEIGHT / WINDOW_HEIGHT_SNES)));
const short SCREEN_WIDTH = WINDOW_WIDTH_SNES * SCREEN_SCALE;		// \ Size of the 8:7 'play-screen'
const short SCREEN_HEIGHT = WINDOW_HEIGHT_SNES * SCREEN_SCALE;		// /
const short BLOCKSIZE = 16 * SCREEN_SCALE;		// The output size of a SNES 16x16 block