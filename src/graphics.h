#pragma once
#include <SDL.h>

/**
 *  \file graphics.h
 *
 *  \brief Handles rendering of objects, window/resolution changes and properties, and framerate
 */
class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize();

	int WindowToggleFullscreen();

	SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
	Uint32 GetSurfacePixel(SDL_Surface* surface, int x, int y);
	int BlitColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int BlitTexture(SDL_Texture* texture, SDL_Rect* source_rect, SDL_Rect* dest_rect);
	int FlipRenderer();

private:
	bool is_fullscreen_;
	SDL_Window* window_main_;
	SDL_Renderer* renderer_main_;
	SDL_Rect screen_rect_;
};