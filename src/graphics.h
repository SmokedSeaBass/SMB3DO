#pragma once

#if defined(_WIN32 )
	#include <SDL.h>
	#include <SDL_image.h>
#elif __linux__
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>
#endif
#include <map>
#include <string>
#include "constants.h"
#include "options.h"
#include "rectangle.h"

/// @brief Handles rendering of objects, window/resolution changes and properties, and framerate.
class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize(Options& options);

	std::pair<unsigned short, unsigned short> GetWindowSize();
	int WindowToggleFullscreen(Options& options);
	void WindowSetTitle(const std::string& subtitle);
	int SetViewport(SDL_Rect& rect);
	const SDL_Rect& GetViewport();
	void UpdateViewport(Options& options);
	std::pair<float, float> GetWindowFitViewportScaler(Options& options, SDL_Rect viewport = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES });

	int BuildDefaultTexture();
	SDL_Texture* GetDefaultTexture();
	/// @brief Creates an SDL_Texture from an SDL_Surface, and then frees the SDL_Surface
	/// @param surface Pointer to the SDL_Surface to create a SDL_Texture from
	/// @return Pointer to an SDL_Texture
	SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
	SDL_Texture* LoadTextureFromImage(const std::string& file_path);
	SDL_Texture* LoadTextureFromImage(const std::string& file_path, Uint32 color_key);
	SDL_Texture* LoadTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y);
	Uint32 GetSurfacePixel(SDL_Surface* surface, int x, int y);
	int DrawColoredRect(const SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredRect(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredLine(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredOutline(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredOutline(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawTexture(SDL_Texture* texture, const SDL_Rect* source_rect, const SDL_Rect* dest_rect, const SDL_RendererFlip flip = SDL_FLIP_NONE);
	int FlipRenderer();

private:
	SDL_Window* window_main_;
	SDL_Renderer* renderer_main_;
	typedef std::map<std::string, SDL_Texture*> TextureList;
	TextureList textures_;
	
	bool is_fullscreen_;
	std::pair<float, float> current_resolution_;
	SDL_Rect viewport_rect_;
	std::pair<float, float> viewport_scaler_;
	std::pair<unsigned short, unsigned short> viewport_ratio_;
};