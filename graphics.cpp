#include "graphics.h"

#include <stdio.h>
#include "SDL.h"
#include "constants.h"

Graphics::Graphics() {
	rendererMain = NULL;
	windowMain = NULL;
	screenRect = { (WINDOW_WIDTH - static_cast<int>(round(WINDOW_WIDTH_NES * SCREEN_SCALE_X))) / 2,
		(WINDOW_HEIGHT - static_cast<int>(round(WINDOW_HEIGHT_NES * SCREEN_SCALE_Y))) / 2,
		static_cast<int>(round(WINDOW_WIDTH_NES * SCREEN_SCALE_X)),
		static_cast<int>(round(WINDOW_HEIGHT_NES * SCREEN_SCALE_Y)) };
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(rendererMain);
	SDL_DestroyWindow(windowMain);
}

int Graphics::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
		return -1;
	}
	windowMain = SDL_CreateWindow("SMB3DO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (windowMain == NULL) {
		printf("ERROR: Main window could not be created: %s\n", SDL_GetError());
		return -1;
	}
	rendererMain = SDL_CreateRenderer(windowMain, -1, SDL_RENDERER_ACCELERATED);
	if (rendererMain == NULL) {
		printf("ERROR: Main renderer could not be created: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);  // Use nearest pixel scaling
	SDL_RenderSetViewport(rendererMain, &screenRect);  // This should be the last rendering operation done UNSCALED; the coordinates and rect are real-sized
	printf("Viewport Absolute Dimensions: %i x %i\n", screenRect.w, screenRect.h);
	SDL_RenderSetScale(rendererMain, SCREEN_SCALE_X, SCREEN_SCALE_Y);  // All drawing isss now scaled from a 256x224 canvas
	return 0;
}

void Graphics::close() {
	SDL_DestroyRenderer(this->rendererMain);
	SDL_DestroyWindow(this->windowMain);
}

int Graphics::renderColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(this->rendererMain, red, green, blue, alpha);
	SDL_RenderFillRect(this->rendererMain, rect);
	return 0;
}

int Graphics::flipRenderer() {
	SDL_RenderPresent(rendererMain);
	return 0;
}