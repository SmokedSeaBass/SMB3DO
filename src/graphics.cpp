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
	if (window_main_ == NULL) {
		printf("ERROR: Main window could not be created: %s\n", SDL_GetError());
		return -1;
	}
	renderer_main_ = SDL_CreateRenderer(window_main_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer_main_ == NULL) {
		printf("ERROR: Main renderer could not be created: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // Pixels!
	//SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);  // Allow for transparency
	SDL_RenderSetViewport(renderer_main_, &screen_rect_);  // This should be the last rendering operation done SCALED; the coordinates and rect are real-sized.  Ideally, this would use canvas coordinates.
	if (META_DEBUG) printf("Window Absolute Dimensions: %d x %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
	if (META_DEBUG) printf("Viewport Absolute Dimensions: %f x %f\n", screen_rect_.w, screen_rect_.h);
	//SDL_RenderSetIntegerScale(renderer_main_, SDL_FALSE);
	//SDL_RenderSetLogicalSize(renderer_main_, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES);
	SDL_RenderSetScale(renderer_main_, SCREEN_SCALE_X, SCREEN_SCALE_Y);  // All drawing is now scaled from a 256x224 canvas
	return 0;
} 

void Graphics::Close() {
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::WindowToggleFullscreen() {
	is_fullscreen_ = !is_fullscreen_;
	if (is_fullscreen_) {
		SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN);
	} else {
		SDL_SetWindowFullscreen(window_main_, 0);
	}
	SDL_RenderSetScale(renderer_main_, 1, 1);			   // \ Currently, this is to properly set the viewport coordinates but after scaling has done, since switching
	SDL_RenderSetViewport(renderer_main_, &screen_rect_);  // / fullscreen loses the viewport's properties.  Ideally, we would switch viewport coordinates to unscaled coordinates
	SDL_RenderSetScale(renderer_main_, SCREEN_SCALE_X, SCREEN_SCALE_Y);
	return 0;
}

SDL_Texture* Graphics::CreateTextureFromSurface(SDL_Surface* surface) {
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
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