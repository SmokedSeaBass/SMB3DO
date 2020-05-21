#pragma once
#include "SDL.h"

class Graphics {
	SDL_Window* windowMain;
	SDL_Renderer* rendererMain;
	SDL_Rect screenRect;

	

public:
	Graphics();
	~Graphics();

	int init();
	void close();

	int renderColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

	int flipRenderer();
};