#pragma once

#if defined(_WIN32)
	#include <SDL.h>
#elif __linux__
	#include <SDL2/SDL.h>
#endif
#include <string>

/// <summary>
/// Handles rendering of objects, window/resolution changes and properties, and framerate.
/// </summary>
class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize();

	int WindowToggleFullscreen();

	SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
	SDL_Texture* CreateTextureFromImage(const std::string& file_path);
	SDL_Texture* CreateTextureFromImage(const std::string& file_path, Uint32 color_key);
	SDL_Texture* CreateTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y);
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