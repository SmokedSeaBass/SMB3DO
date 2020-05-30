#include "graphics.h"

#include <stdio.h>
#include "SDL.h"
#include "constants.h"

Graphics::Graphics() {
	is_fullscreen_ = false;
	renderer_main_ = NULL;
	window_main_ = NULL;
	screen_rect_ = {
		/* TODO: Use canvas coordinates */
		(WINDOW_WIDTH - static_cast<int>(round(WINDOW_WIDTH_NES * SCREEN_SCALE_X))) / 2,
		(WINDOW_HEIGHT - static_cast<int>(round(WINDOW_HEIGHT_NES * SCREEN_SCALE_Y))) / 2,
		static_cast<int>(round(WINDOW_WIDTH_NES * SCREEN_SCALE_X)),
		static_cast<int>(round(WINDOW_HEIGHT_NES * SCREEN_SCALE_Y)) 
	};
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::Initialize() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
		return -1;
	}

	window_main_ = SDL_CreateWindow(
		"SMB3DO - v" META_VERSION,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (window_main_ == nullptr) {
		printf("ERROR: Main window could not be created: %s\n", SDL_GetError());
		return -1;
	}

	renderer_main_ = SDL_CreateRenderer(window_main_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer_main_ == nullptr) {
		printf("ERROR: Main renderer could not be created: %s\n", SDL_GetError());
		return -1;
	}
	
	// Pixel uspcale, no softening or antialias
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest") == SDL_FALSE) {
		printf("ERROR: Render scale quality hint could not be set: %s\n", SDL_GetError());
		return -1;
	}

	SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);  // Allow for alpha transparency

	// This should be the last rendering operation done SCALED; the coordinates and rect are real-sized.  Ideally, this would use canvas coordinates.
	if (SDL_RenderSetViewport(renderer_main_, &screen_rect_) < 0) {
		printf("ERROR: Main viewport could not be created: %s\n", SDL_GetError());
		return -1;  
	} 

	if (META_DEBUG) printf("Window Absolute Dimensions: %d x %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
	if (META_DEBUG) printf("Viewport Absolute Dimensions: %d x %d\n", screen_rect_.w, screen_rect_.h);

	// All drawing should be scaled from a 256x224 canvas
	if (SDL_RenderSetScale(renderer_main_, SCREEN_SCALE_X, SCREEN_SCALE_Y) < 0) {
		printf("ERROR: Main renderer scaling could not be set: %s\n", SDL_GetError());
		return -1;
	}

	// Alternative scaling process.  Automatically creates 1-to-1 pixel ratio, instead of 4:3
	//SDL_RenderSetIntegerScale(renderer_main_, SDL_FALSE);
	//SDL_RenderSetLogicalSize(renderer_main_, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES);

	return 0;
}

int Graphics::WindowToggleFullscreen() {
	is_fullscreen_ = !is_fullscreen_;
	if (is_fullscreen_) {
		if (SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN) < 0) {
			printf("ERROR: Could not switch to fullscreen mode: %s\n", SDL_GetError());
			return -1;
		}
	} else {
		if (SDL_SetWindowFullscreen(window_main_, 0) < 0) {
			printf("ERROR: Could not switch to windowed mode: %s\n", SDL_GetError());
			return -1;
		}
	}

	// Currently, this is to properly set the viewport coordinates after scaling has
	// already been done, since switching to/from fullscreen losses the viewport's properties.
	// TODO: Ideally, we would switch viewport coordinates to unscaled coordinates to prevent this "hack".
	if (SDL_RenderSetScale(renderer_main_, 1, 1) < 0) {
		printf("ERROR: Main renderer scaling could not be set: %s\n", SDL_GetError());
		return -1;
	}
	if (SDL_RenderSetViewport(renderer_main_, &screen_rect_) < 0) {
		printf("ERROR: Main viewport could not be created: %s\n", SDL_GetError());
		return -1;
	}
	if (SDL_RenderSetScale(renderer_main_, SCREEN_SCALE_X, SCREEN_SCALE_Y) < 0) {
		printf("ERROR: Main renderer scaling could not be set: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

SDL_Texture* Graphics::CreateTextureFromSurface(SDL_Surface* surface) {
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
}

// Taken from StackOverflow: https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
Uint32 Graphics::GetSurfacePixel(SDL_Surface* surface, int x, int y) {
	SDL_LockSurface(surface);
	int bpp = surface->format->BytesPerPixel;
	// Get address of the pixel we want to retrieve
	Uint8* ptr = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	Uint32 pixel = 0x00000000;

	switch (bpp) {
	case 1:
		pixel = *ptr;
		break;
	case 2:
		pixel = *(Uint16*)ptr;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			pixel =  ptr[0] << 16 | ptr[1] << 8 | ptr[2];
		} else {
			pixel = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
		}
		break;
	case 4:
		pixel = *(Uint32*)ptr;
		break;
	default:
		break;
	}

	SDL_UnlockSurface(surface);
	return pixel;
}

int Graphics::BlitColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	SDL_RenderFillRect(renderer_main_, rect);
	return 0;
}

int Graphics::BlitTexture(SDL_Texture* texture, SDL_Rect* source_rect, SDL_Rect* dest_rect) {
	return SDL_RenderCopy(renderer_main_, texture, source_rect, dest_rect);
}

int Graphics::FlipRenderer() {
	SDL_RenderPresent(renderer_main_);
	return 0;
}