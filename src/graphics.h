#pragma once
#include <SDL.h>

class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize();
	void Close();

	int WindowToggleFullscreen();

	SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
	int BlitColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int BlitTexture(SDL_Texture* texture, SDL_Rect* source_rect, SDL_Rect* dest_rect);
	int FlipRenderer();

private:
	bool is_fullscreen_;
	SDL_Window* window_main_;
	SDL_Renderer* renderer_main_;
	SDL_Rect screen_rect_;
};